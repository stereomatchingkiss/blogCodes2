#include <dlib/dnn.h>
#include <dlib/image_io.h>
#include <dlib/misc_api.h>

using namespace dlib;
using namespace std;

// ----------------------------------------------------------------------------------------

// We will need to create some functions for loading data.  This program will
// expect to be given a directory structured as follows:
//    top_level_directory/
//        person1/
//            image1.jpg
//            image2.jpg
//            image3.jpg
//        person2/
//            image4.jpg
//            image5.jpg
//            image6.jpg
//        person3/
//            image7.jpg
//            image8.jpg
//            image9.jpg
//
// The specific folder and image names don't matter, nor does the number of folders or
// images.  What does matter is that there is a top level folder, which contains
// subfolders, and each subfolder contains images of a single person.

// This function spiders the top level directory and obtains a list of all the
// image files.
std::vector<std::vector<string>> load_objects_list (
    const string& dir
)
{
    std::vector<std::vector<string>> objects;
    for (auto subdir : directory(dir).get_dirs())
    {
        cout<<"loading:"<<subdir.full_name()<<endl;
        std::vector<string> imgs;
        for (auto img : subdir.get_files())
            imgs.push_back(img);

        if (imgs.size() != 0)
            objects.push_back(imgs);
    }
    return objects;
}

rectangle make_random_cropping_rect_resnet(
    const matrix<rgb_pixel>& img,
    dlib::rand& rnd
)
{
    // figure out what rectangle we want to crop from the image
    double mins = 0.7, maxs = 0.95;
    auto const scale = mins + rnd.get_random_double()*(maxs-mins);
    auto const size = scale*std::min(img.nr(), img.nc());
    rectangle const rect(size, size);
    // randomly shift the box around
    point const offset(rnd.get_random_32bit_number()%(img.nc()-rect.width()),
                       rnd.get_random_32bit_number()%(img.nr()-rect.height()));
    return move_rect(rect, offset);
}

void randomly_crop_image (
    const matrix<rgb_pixel>& img,
    matrix<rgb_pixel>& crop,
    dlib::rand& rnd
)
{
    auto const rect = make_random_cropping_rect_resnet(img, rnd);

    // now crop it out as a 150x150 image.
    extract_image_chip(img, chip_details(rect, chip_dims(150,150)), crop);
}

// This function takes the output of load_objects_list() as input and randomly
// selects images for training.  It should also be pointed out that it's really
// important that each mini-batch contain multiple images of each person.  This
// is because the metric learning algorithm needs to consider pairs of images
// that should be close (i.e. images of the same person) as well as pairs of
// images that should be far apart (i.e. images of different people) during each
// training step.
void load_mini_batch (
    const size_t num_people,     // how many different people to include
    const size_t samples_per_id, // how many images per person to select.
    dlib::rand& rnd,
    const std::vector<std::vector<string>>& objs,
    std::vector<matrix<rgb_pixel>>& images,
    std::vector<unsigned long>& labels
)
{
    images.clear();
    labels.clear();
    DLIB_CASSERT(num_people <= objs.size(), "The dataset doesn't have that many people in it.");

    std::vector<bool> already_selected(objs.size(), false);
    matrix<rgb_pixel> image;
    for (size_t i = 0; i < num_people; ++i)
    {
        size_t id = rnd.get_random_32bit_number()%objs.size();
        // don't pick a person we already added to the mini-batch
        while(already_selected[id])
            id = rnd.get_random_32bit_number()%objs.size();
        already_selected[id] = true;

        std::vector<bool> faces_selected(objs[id].size(), false);
        for (size_t j = 0; j < samples_per_id; ++j)
        {
            size_t face_id = rnd.get_random_32bit_number()%objs[id].size();
            while(faces_selected[face_id]){
                face_id = rnd.get_random_32bit_number()%objs[id].size();
            }
            faces_selected[face_id] = true;
            const auto& obj = objs[id][face_id];
            load_image(image, obj);
            matrix<rgb_pixel> crop_image;
            randomly_crop_image(image, crop_image, rnd);
            images.emplace_back(std::move(crop_image));
            labels.emplace_back(id);
        }
    }

    // You might want to do some data augmentation at this point.  Here we do some simple
    // color augmentation.
    for (auto&& crop : images)
    {
        disturb_colors(crop,rnd);
        // Jitter most crops
        if (rnd.get_random_double() > 0.1){
            crop = jitter_image(crop,rnd);
        }
        if (rnd.get_random_double() > 0.5){
            flip_image_left_right(crop);
        }
    }

    // All the images going into a mini-batch have to be the same size.  And really, all
    // the images in your entire training dataset should be the same size for what we are
    // doing to make the most sense.
    DLIB_CASSERT(images.size() > 0);
    for (auto&& img : images)
    {
        DLIB_CASSERT(img.nr() == images[0].nr() && img.nc() == images[0].nc(),
            "All the images in a single mini-batch must be the same size.");
    }
}

// ----------------------------------------------------------------------------------------

// The next page of code defines a ResNet network.  It's basically copied
// and pasted from the dnn_imagenet_ex.cpp example, except we replaced the loss
// layer with loss_metric and make the network somewhat smaller.

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = add_prev1<block<N,BN,1,tag1<SUBNET>>>;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = add_prev2<avg_pool<2,2,2,2,skip1<tag2<block<N,BN,2,tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET>
using block  = BN<con<N,3,3,1,1,relu<BN<con<N,3,3,stride,stride,SUBNET>>>>>;

template <int N, typename SUBNET> using res      = relu<residual<block,N,bn_con,SUBNET>>;
template <int N, typename SUBNET> using ares      = relu<residual<block,N,affine,SUBNET>>;
template <int N, typename SUBNET> using res_down = relu<residual_down<block,N,bn_con,SUBNET>>;
template <int N, typename SUBNET> using ares_down = relu<residual_down<block,N,affine,SUBNET>>;

template <typename SUBNET> using level0 = res_down<256,SUBNET>;
template <typename SUBNET> using level1 = res<256,res<256,res_down<256,SUBNET>>>;
template <typename SUBNET> using level2 = res<128,res<128,res_down<128,SUBNET>>>;
template <typename SUBNET> using level3 = res<64,res<64,res<64,res_down<64,SUBNET>>>>;
template <typename SUBNET> using level4 = res<32,res<32,res<32,SUBNET>>>;

template <typename SUBNET> using alevel0 = ares_down<256,SUBNET>;
template <typename SUBNET> using alevel1 = ares<256,ares<256,ares_down<256,SUBNET>>>;
template <typename SUBNET> using alevel2 = ares<128,ares<128,ares_down<128,SUBNET>>>;
template <typename SUBNET> using alevel3 = ares<64,ares<64,ares<64,ares_down<64,SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32,ares<32,ares<32,SUBNET>>>;

// training network type
using net_type = loss_metric<fc_no_bias<128,avg_pool_everything<
                            level0<
                            level1<
                            level2<
                            level3<
                            level4<
                            max_pool<3,3,2,2,relu<bn_con<con<32,7,7,2,2,
                            input_rgb_image_sized<150>
                            >>>>>>>>>>>>;

using anet_type = loss_metric<fc_no_bias<128,avg_pool_everything<
                            alevel0<
                            alevel1<
                            alevel2<
                            alevel3<
                            alevel4<
                            max_pool<3,3,2,2,relu<affine<con<32,7,7,2,2,
                            input_rgb_image_sized<150>
                            >>>>>>>>>>>>;
// ----------------------------------------------------------------------------------------

template<typename Net>
inline
decltype(std::declval<Net>().layer_details().set_learning_rate_multiplier(0))
set_learning_rate_impl(Net &net, double rate)
{
    net.layer_details().set_learning_rate_multiplier(rate);
    net.layer_details().set_bias_learning_rate_multiplier(rate);
}

inline
void set_learning_rate_impl(...)
{
}

template<int from, int to, typename Net>
typename std::enable_if<from == to>::type
set_learning_rate(Net&, double)
{
}

template<int from, int to, typename Net>
typename std::enable_if<from != to>::type
set_learning_rate(Net &net, double rate)
{
    set_learning_rate_impl(dlib::layer<from>(net), rate);
    set_learning_rate<from + 1, to>(net, rate);
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cout << "Give a folder as input.  It should contain sub-folders of images and we will " << endl;
        cout << "learn to distinguish between these sub-folders with metric learning.  " << endl;
        cout << "For example, you can run this program on the very small examples/johns dataset" << endl;
        cout << "that comes with dlib by running this command:" << endl;
        cout << "   ./dnn_metric_learning_on_images_ex johns" << endl;
        return 1;
    }

    auto objs = load_objects_list(argv[1]);

    cout << "objs.size(): "<< objs.size() << endl;

    std::vector<matrix<rgb_pixel>> images;
    std::vector<unsigned long> labels;

    anet_type net;
    cout<<net<<endl;
    anet_type copy_net;
    cout<<"load network"<<endl;
    deserialize("../../computer_vision_model/dlib/dlib_face_recognition_resnet_model_v1.dat")>>copy_net;
    cout<<"load network end"<<endl;
    int constexpr copy_until = 7;
    layer<copy_until>(net) = layer<copy_until>(copy_net);
    cout<<"set learning rate to 0"<<endl;
    set_learning_rate<copy_until, 130>(net, 0.0);

    dnn_trainer<anet_type> trainer(net, sgd(0.0001, 0.9));
    trainer.set_learning_rate(0.0001);
    trainer.be_verbose();
    trainer.set_synchronization_file("face_metric_sync", std::chrono::minutes(5));
    // I've set this to something really small to make the example terminate
    // sooner.  But when you really want to train a good model you should set
    // this to something like 10000 so training doesn't terminate too early.
    trainer.set_iterations_without_progress_threshold(2000);

    using image_info = std::pair<std::vector<matrix<rgb_pixel>>, std::vector<unsigned long>>;
    // If you have a lot of data then it might not be reasonable to load it all
    // into RAM.  So you will need to be sure you are decompressing your images
    // and loading them fast enough to keep the GPU occupied.  I like to do this
    // using the following coding pattern: create a bunch of threads that dump
    // mini-batches into dlib::pipes.
    //dlib::pipe<std::vector<matrix<rgb_pixel>>> qimages(4);
    //dlib::pipe<std::vector<unsigned long>> qlabels(4);
    dlib::pipe<image_info> image_info_pipe(8);
    auto data_loader = [&image_info_pipe, &objs](time_t seed)
    {
        dlib::rand rnd(time(0)+seed);
        std::vector<matrix<rgb_pixel>> images;
        std::vector<unsigned long> labels;
        while(image_info_pipe.is_enabled())
        {
            try
            {
                load_mini_batch(35, 16, rnd, objs, images, labels);
                image_info_pipe.enqueue(std::make_pair(std::move(images), std::move(labels)));
            }
            catch(std::exception& e)
            {
                cout << "EXCEPTION IN LOADING DATA" << endl;
                cout << e.what() << endl;
            }
        }
    };
    // Run the data_loader from 4 threads.  You should set the number of threads
    // relative to the number of CPU cores you have.
    cout<<"create data loader"<<endl;
    std::thread data_loader1([data_loader](){ data_loader(1); });
    std::thread data_loader2([data_loader](){ data_loader(2); });
    std::thread data_loader3([data_loader](){ data_loader(3); });
    std::thread data_loader4([data_loader](){ data_loader(4); });


    // Here we do the training.  We keep passing mini-batches to the trainer until the
    // learning rate has dropped low enough.
    cout<<"start training"<<endl;
    while(trainer.get_learning_rate() >= 1e-7)
    {
        images.clear();
        labels.clear();
        while(images.empty()){
            if(image_info_pipe.is_dequeue_enabled()){
                image_info image_info_item;
                image_info_pipe.dequeue(image_info_item);
                images = std::move(image_info_item.first);
                labels = std::move(image_info_item.second);
            }
        }
        trainer.train_one_step(images, labels);
    }

    // Wait for training threads to stop
    trainer.get_net();
    cout << "done training" << endl;

    // Save the network to disk
    net.clean();
    serialize("metric_network_renset.dat") << net;

    // stop all the data loading threads and wait for them to terminate.
    image_info_pipe.disable();
    data_loader1.join();
    data_loader2.join();
    data_loader3.join();
    data_loader4.join();

    // Now, just to show an example of how you would use the network, let's check how well
    // it performs on the training data.
    dlib::rand rnd(time(0));
    load_mini_batch(35, 15, rnd, objs, images, labels);

    // Normally you would use the non-batch-normalized version of the network to do
    // testing, which is what we do here.
    anet_type testing_net = net;

    // Run all the images through the network to get their vector embeddings.
    std::vector<matrix<float,0,1>> embedded = testing_net(images);

    // Now, check if the embedding puts images with the same labels near each other and
    // images with different labels far apart.
    int num_right = 0;
    int num_wrong = 0;
    for (size_t i = 0; i < embedded.size(); ++i)
    {
        for (size_t j = i+1; j < embedded.size(); ++j)
        {
            if (labels[i] == labels[j])
            {
                // The loss_metric layer will cause images with the same label to be less
                // than net.loss_details().get_distance_threshold() distance from each
                // other.  So we can use that distance value as our testing threshold.
                if (length(embedded[i]-embedded[j]) < testing_net.loss_details().get_distance_threshold())
                    ++num_right;
                else
                    ++num_wrong;
            }
            else
            {
                if (length(embedded[i]-embedded[j]) >= testing_net.loss_details().get_distance_threshold())
                    ++num_right;
                else
                    ++num_wrong;
            }
        }
    }

    cout << "num_right: "<< num_right << endl;
    cout << "num_wrong: "<< num_wrong << endl;//*/
}
