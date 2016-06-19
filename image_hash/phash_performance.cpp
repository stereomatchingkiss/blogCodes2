#include <opencv2/highgui.hpp>
#include <opencv2/img_hash.hpp>
#include <opencv2/imgproc.hpp>

#include "pHash.h"
#include <CImg.h>

#include <chrono>
#include <iostream>

using namespace cimg_library;
using namespace cv;

#ifdef max
#undef max
#endif

typedef std::chrono::time_point<std::chrono::system_clock> time_point;

std::vector<CImg<uint8_t>> read_img();

int ph_image_digest(const CImg<uint8_t> &img,double sigma,
                    double gamma,Digest &digest, int N = 180);

uint8_t* ph_mh_imagehash(CImg<uint8_t> &src, int &N,
                         float alpha = 2.0f,
                         float lvl = 1.0f);

void ph_bmb_free(BinHash *bh);

BinHash* _ph_bmb_new(uint32_t bytelength);

int ph_bmb_imagehash(CImg<uint8_t> &img, uint8_t method,
                     BinHash **ret_hash);

double ph_hammingdistance2(uint8_t *hashA, int lenA,
                           uint8_t *hashB, int lenB);

int ph_crosscorr(const Digest &x,const Digest &y,
                 double &pcc,double threshold);

void test_block_mean_hash(uint8_t method);
void test_marr_hash();
void test_radial_hash();

void test_block_mean_hash_compare(uint8_t method);
void test_marr_hash_compare();
void test_radial_hash_compare();

int main()
{
    //test_block_mean_hash(1);
    //test_block_mean_hash(2);
    //test_marr_hash();
    //test_radial_hash();

    //test_block_mean_hash_compare(1);
    //test_block_mean_hash_compare(2);
    //test_marr_hash_compare();
    test_radial_hash_compare();
}

void test_radial_hash_compare()
{
    std::vector<CImg<uint8_t>> imgs = read_img();
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::cout<<"test radial hash comparison : ";
    Digest digest;
    std::vector<Digest> digests;
    for(size_t i = 0; i != imgs.size(); ++i)
    {
        digest.coeffs = 0;
        ph_image_digest(imgs[i], 1.0, 1.0, digest);
        if(digest.coeffs)
        {
            digests.push_back(digest);
        }
        else
        {
            throw std::runtime_error("hash computation failed");
        }
    }
    start = std::chrono::system_clock::now();
    double pcc = 0;
    for(size_t i = 0; i != digests.size(); ++i)
    {
        ph_crosscorr(digests[0], digests[i], pcc, 0.9);
    }
    end = std::chrono::system_clock::now();;
    std::cout<<std::chrono::duration_cast<std::chrono::microseconds>
               (end - start).count()<<std::endl;
    for(size_t i = 0; i != digests.size(); ++i)
    {
        free(digests[i].coeffs);
    }
}

void test_marr_hash_compare()
{
    std::vector<CImg<uint8_t>> imgs = read_img();
    std::cout<<"test Marr hash : ";
    time_point start, end;
    std::vector<uint8_t*> hashes;
    for(size_t i = 0; i != imgs.size(); ++i)
    {
        int N = 0;
        uint8_t *hash = ph_mh_imagehash(imgs[i], N);
        if(hash)
        {
            hashes.emplace_back(hash);
        }
        else
        {
            throw std::runtime_error("hash computation failed");
        }
    }
    start = std::chrono::system_clock::now();
    for(size_t i = 0; i != hashes.size(); ++i)
    {
        //if you do not assign the value, compiler
        //may not do anything in this loop
        double const value = ph_hammingdistance2(hashes[0], 72, hashes[i], 72);
    }
    end = std::chrono::system_clock::now();;
    std::cout<<std::chrono::duration_cast<std::chrono::microseconds>
               (end - start).count()<<std::endl;
    for(size_t i = 0; i != hashes.size(); ++i)
    {
        free(hashes[i]);
    }
}

void test_block_mean_hash_compare(uint8_t method)
{
    std::vector<CImg<uint8_t>> imgs = read_img();
    std::cout<<"test block mean hash method "<<(method - 1)<<" : ";

    std::vector<BinHash*> hashs;
    for(size_t i = 0; i != imgs.size(); ++i)
    {
        BinHash *hash = 0;
        ph_bmb_imagehash(imgs[i], method, &hash);
        if(hash)
        {
            hashs.emplace_back(hash);
        }
        else
        {
            throw std::runtime_error("hash conputation fail");
        }
    }

    time_point start, end;
    start = std::chrono::system_clock::now();
    for(size_t i = 0; i != hashs.size(); ++i)
    {
        //if you do not assign the value, compiler
        //may not do anything in this loop
        double const value = ph_hammingdistance2(hashs[0]->hash,
                hashs[0]->bytelength,
                hashs[i]->hash,
                hashs[i]->bytelength);
        //std::cout<<i<<" : "<<value<<std::endl;
    }
    end = std::chrono::system_clock::now();;
    std::cout<<std::chrono::duration_cast<std::chrono::microseconds>
               (end - start).count()<<std::endl;
    for(size_t i = 0; i != hashs.size(); ++i)
    {
        ph_bmb_free(hashs[i]);
    }
}

void test_block_mean_hash(uint8_t method)
{
    std::vector<CImg<uint8_t>> imgs = read_img();
    std::cout<<"test block mean hash method "<<(method - 1)<<std::endl;
    time_point start, end;
    start = std::chrono::system_clock::now();
    for(size_t i = 0; i != imgs.size(); ++i)
    {
        BinHash *hash = 0;
        ph_bmb_imagehash(imgs[i], method, &hash);
        if(hash)
        {
            ph_bmb_free(hash);
        }
    }
    end = std::chrono::system_clock::now();;
    std::cout<<std::chrono::duration_cast<std::chrono::microseconds>
               (end - start).count()<<std::endl;
}

void test_marr_hash()
{
    std::vector<CImg<uint8_t>> imgs = read_img();
    std::cout<<"test Marr hash"<<std::endl;
    time_point start, end;
    start = std::chrono::system_clock::now();
    for(size_t i = 0; i != imgs.size(); ++i)
    {
        int N = 0;
        uint8_t *hash = ph_mh_imagehash(imgs[i], N);
        if(hash)
        {
            free(hash);
        }
    }
    end = std::chrono::system_clock::now();;
    std::cout<<std::chrono::duration_cast<std::chrono::microseconds>
               (end - start).count()<<std::endl;
}

void test_radial_hash()
{
    std::vector<CImg<uint8_t>> imgs = read_img();
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::cout<<"test radial hash"<<std::endl;
    start = std::chrono::system_clock::now();
    Digest digest;
    for(size_t i = 0; i != imgs.size(); ++i)
    {
        digest.coeffs = 0;
        ph_image_digest(imgs[i], 1.0, 1.0, digest);
        if(digest.coeffs)
        {
            free(digest.coeffs);
        }
    }
    end = std::chrono::system_clock::now();;
    std::cout<<std::chrono::duration_cast<std::chrono::microseconds>
               (end - start).count()<<std::endl;
}

void ph_bmb_free(BinHash *bh)
{
    if(bh)
    {
        free(bh->hash);
        free(bh);
    }
}

//int ph_bmb_imagehash(CImg<uint8_t> &img, uint8_t method, BinHash **ret_hash);
int ph_bmb_imagehash(CImg<uint8_t> &img, uint8_t method, BinHash **ret_hash)
{
    const uint8_t *ptrsrc;  // source pointer (img)
    uint8_t *block;
    int pcol;  // "pointer" to pixel col (x)
    int prow;  // "pointer" to pixel row (y)
    int blockidx = 0;  //current idx of block begin processed.
    double median;  // median value of mean_vals
    const int preset_size_x=256;
    const int preset_size_y=256;
    const int blk_size_x=16;
    const int blk_size_y=16;
    int pixcolstep = blk_size_x;
    int pixrowstep = blk_size_y;

    int number_of_blocks;
    uint32_t bitsize;
    // number of bytes needed to store bitsize bits.
    uint32_t bytesize;

    if (!ret_hash){
        return -1;
    }

    const int blk_size = blk_size_x * blk_size_y;
    block = (uint8_t*)malloc(sizeof(uint8_t) * blk_size);

    if(!block)
        return -1;

    switch (img.spectrum()) {
    case 3: // from RGB
        img.RGBtoYCbCr().channel(0);
        break;
    default:
        *ret_hash = NULL;
        free(block);
        return -1;
    }

    img.resize(preset_size_x, preset_size_y);

    // ~step b
    ptrsrc = img.data();  // set pointer to beginning of pixel buffer

    if(method == 2)
    {
        pixcolstep /= 2;
        pixrowstep /= 2;

        number_of_blocks =
                ((preset_size_x / blk_size_x) * 2 - 1) *
                ((preset_size_y / blk_size_y) * 2 - 1);
    } else {
        number_of_blocks =
                preset_size_x / blk_size_x *
                preset_size_y / blk_size_y;
    }

    bitsize= number_of_blocks;
    bytesize = bitsize / 8;

    double *mean_vals = new double[number_of_blocks];

    /*
    * pixel row < block < block row < image
    *
    * The pixel rows of a block are copied consecutively
    * into the block buffer (using memcpy). When a block is
    * finished, the next block in the block row is processed.
    * After finishing a block row, the processing of the next
    * block row is started. An image consists of an arbitrary
    * number of block rows.
    */

    /* image (multiple rows of blocks) */
    for(prow = 0;prow<=preset_size_y-blk_size_y;prow += pixrowstep)
    {

        /* block row */
        for(pcol = 0;pcol<=preset_size_x-blk_size_x;pcol += pixcolstep)
        {

            // idx for array holding one block.
            int blockpos = 0;

            /* block */

            // i is used to address the different
            // pixel rows of a block
            for(int i=0 ; i < blk_size_y; i++)
            {
                ptrsrc = img.data(pcol, prow + i);
                memcpy(block + blockpos, ptrsrc, blk_size_x);
                blockpos += blk_size_x;
            }

            mean_vals[blockidx] = CImg<uint8_t>(block,blk_size).mean();
            blockidx++;

        }
    }

    /* calculate the median */
    median = CImg<double>(mean_vals, number_of_blocks).median();

    /* step e */
    BinHash *hash = _ph_bmb_new(bytesize);

    if(!hash)
    {
        *ret_hash = NULL;
        return -1;
    }

    *ret_hash = hash;
    for(uint32_t i = 0; i < bitsize; i++)
    {
        if(mean_vals[i] < median)
        {
            hash->addbit(0);
        } else {
            hash->addbit(1);
        }
    }
    delete[] mean_vals;
    free(block);
    return 0;
}

BinHash* _ph_bmb_new(uint32_t bytelength)
{
    BinHash* bh = (BinHash*)malloc(sizeof(BinHash));
    bh->bytelength = bytelength;
    bh->hash = (uint8_t*)calloc(sizeof(uint8_t), bytelength);
    bh->byteidx = 0;
    bh->bitmask = 128;
    return bh;
}

int ph_image_digest(const CImg<uint8_t> &img,double sigma, double gamma,Digest &digest, int N){

    int result = EXIT_FAILURE;
    CImg<uint8_t> graysc;
    if (img.spectrum() >= 3){
        graysc = img.get_RGBtoYCbCr().channel(0);
    }
    else if (img.spectrum() == 1){
        graysc = img;
    }
    else {
        return result;
    }


    graysc.blur((float)sigma);

    (graysc/graysc.max()).pow(gamma);

    Projections projs;
    if (ph_radon_projections(graysc,N,projs) < 0)
        goto cleanup;

    Features features;
    if (ph_feature_vector(projs,features) < 0)
        goto cleanup;

    if (ph_dct(features,digest) < 0)
        goto cleanup;

    result = EXIT_SUCCESS;

cleanup:
    free(projs.nb_pix_perline);
    free(features.features);

    delete projs.R;
    return result;
}

int ph_feature_vector(const Projections &projs, Features &fv)
{

    CImg<uint8_t> *ptr_map = projs.R;
    CImg<uint8_t> projection_map = *ptr_map;
    int *nb_perline = projs.nb_pix_perline;
    int N = projs.size;
    int D = projection_map.height();

    fv.features = (double*)malloc(N*sizeof(double));
    fv.size = N;
    if (!fv.features)
        return EXIT_FAILURE;

    double *feat_v = fv.features;
    double sum = 0.0;
    double sum_sqd = 0.0;
    for (int k=0; k < N; k++){
        double line_sum = 0.0;
        double line_sum_sqd = 0.0;
        int nb_pixels = nb_perline[k];
        for (int i=0;i<D;i++){
            line_sum += projection_map(k,i);
            line_sum_sqd += projection_map(k,i)*projection_map(k,i);
        }
        feat_v[k] = (line_sum_sqd/nb_pixels) - (line_sum*line_sum)/(nb_pixels*nb_pixels);
        sum += feat_v[k];
        sum_sqd += feat_v[k]*feat_v[k];
    }
    double mean = sum/N;
    double var  = sqrt((sum_sqd/N) - (sum*sum)/(N*N));

    for (int i=0;i<N;i++){
        feat_v[i] = (feat_v[i] - mean)/var;
    }

    return EXIT_SUCCESS;
}
int ph_dct(const Features &fv,Digest &digest)
{
    int N = fv.size;
    const int nb_coeffs = 40;

    digest.coeffs = (uint8_t*)malloc(nb_coeffs*sizeof(uint8_t));
    if (!digest.coeffs)
        return EXIT_FAILURE;

    digest.size = nb_coeffs;

    double *R = fv.features;

    uint8_t *D = digest.coeffs;

    double D_temp[nb_coeffs];
    double max = 0.0;
    double min = 0.0;
    for (int k = 0;k<nb_coeffs;k++){
        double sum = 0.0;
        for (int n=0;n<N;n++){
            double temp = R[n]*cos((cimg::PI*(2*n+1)*k)/(2*N));
            sum += temp;
        }
        if (k == 0)
            D_temp[k] = sum/sqrt((double)N);
        else
            D_temp[k] = sum*SQRT_TWO/sqrt((double)N);
        if (D_temp[k] > max)
            max = D_temp[k];
        if (D_temp[k] < min)
            min = D_temp[k];
    }

    for (int i=0;i<nb_coeffs;i++){

        D[i] = (uint8_t)(UCHAR_MAX*(D_temp[i] - min)/(max - min));

    }

    return EXIT_SUCCESS;
}

int ph_crosscorr(const Digest &x,const Digest &y,double &pcc,double threshold){

    int N = y.size;
    int result = 0;

    uint8_t *x_coeffs = x.coeffs;
    uint8_t *y_coeffs = y.coeffs;

    double *r = new double[N];
    double sumx = 0.0;
    double sumy = 0.0;
    for (int i=0;i < N;i++){
        sumx += x_coeffs[i];
        sumy += y_coeffs[i];
    }
    double meanx = sumx/N;
    double meany = sumy/N;
    double max = 0;
    for (int d=0;d<N;d++){
        double num = 0.0;
        double denx = 0.0;
        double deny = 0.0;
        for (int i=0;i<N;i++){
            num  += (x_coeffs[i]-meanx)*(y_coeffs[(N+i-d)%N]-meany);
            denx += pow((x_coeffs[i]-meanx),2);
            deny += pow((y_coeffs[(N+i-d)%N]-meany),2);
        }
        r[d] = num/sqrt(denx*deny);
        if (r[d] > max)
            max = r[d];
    }
    delete[] r;
    pcc = max;
    if (max > threshold)
        result = 1;

    return result;
}

int ph_radon_projections(const CImg<uint8_t> &img,int N,Projections &projs){

    int width = img.width();
    int height = img.height();
    int D = (width > height)?width:height;
    float x_center = (float)width/2;
    float y_center = (float)height/2;
    int x_off = (int)std::floor(x_center + ROUNDING_FACTOR(x_center));
    int y_off = (int)std::floor(y_center + ROUNDING_FACTOR(y_center));

    projs.R = new CImg<uint8_t>(N,D,1,1,0);
    projs.nb_pix_perline = (int*)calloc(N,sizeof(int));

    if (!projs.R || !projs.nb_pix_perline)
        return EXIT_FAILURE;

    projs.size = N;

    CImg<uint8_t> *ptr_radon_map = projs.R;
    int *nb_per_line = projs.nb_pix_perline;

    for (int k=0;k<N/4+1;k++){
        double theta = k*cimg::PI/N;
        double alpha = std::tan(theta);
        for (int x=0;x < D;x++){
            double y = alpha*(x-x_off);
            int yd = (int)std::floor(y + ROUNDING_FACTOR(y));
            if ((yd + y_off >= 0)&&(yd + y_off < height) && (x < width)){
                *ptr_radon_map->data(k,x) = img(x,yd + y_off);
                nb_per_line[k] += 1;
            }
            if ((yd + x_off >= 0) && (yd + x_off < width) && (k != N/4) && (x < height)){
                *ptr_radon_map->data(N/2-k,x) = img(yd + x_off,x);
                nb_per_line[N/2-k] += 1;
            }
        }
    }
    int j= 0;
    for (int k=3*N/4;k<N;k++){
        double theta = k*cimg::PI/N;
        double alpha = std::tan(theta);
        for (int x=0;x < D;x++){
            double y = alpha*(x-x_off);
            int yd = (int)std::floor(y + ROUNDING_FACTOR(y));
            if ((yd + y_off >= 0)&&(yd + y_off < height) && (x < width)){
                *ptr_radon_map->data(k,x) = img(x,yd + y_off);
                nb_per_line[k] += 1;
            }
            if ((y_off - yd >= 0)&&(y_off - yd<width)&&(2*y_off-x>=0)&&(2*y_off-x<height)&&(k!=3*N/4)){
                *ptr_radon_map->data(k-j,x) = img(-yd+y_off,-(x-y_off)+y_off);
                nb_per_line[k-j] += 1;
            }

        }
        j += 2;
    }

    return EXIT_SUCCESS;

}

std::vector<CImg<uint8_t>> read_img()
{
    std::vector<CImg<uint8_t>> imgs(100);
    for(size_t i = 0; i != 100; ++i)
    {
        //CImg come with PHash cannot read jpg, so
        //I read it by opencv
        cv::Mat input = cv::imread("ukbench/ukbench0" +
                                   std::to_string(i + 3000) + ".jpg");
        CImg<uint8_t> img(input.cols, input.rows,
                          1, input.channels());
        for(int c = 0; c != input.channels(); ++c)
        {
            for(int row = 0; row != input.rows; ++row)
            {
                for(int col = 0; col != input.cols; ++col)
                {
                    img(col, row, 0, c) = input.at<cv::Vec3b>(row, col)[c];
                }
            }
        }
        imgs[i] = img;
    }

    return imgs;
}

CImg<float>* GetMHKernel(float alpha, float level){
    int sigma = (int)(4*pow((float)alpha,(float)level));
    static CImg<float> *pkernel = NULL;
    float xpos, ypos, A;
    if (!pkernel){
        pkernel = new CImg<float>(2*sigma+1,2*sigma+1,1,1,0);
        cimg_forXY(*pkernel,X,Y){
            xpos = pow(alpha,-level)*(X-sigma);
            ypos = pow(alpha,-level)*(Y-sigma);
            A = xpos*xpos + ypos*ypos;
            pkernel->atXY(X,Y) = (2-A)*exp(-A/2);
        }
    }
    return pkernel;
}

uint8_t* ph_mh_imagehash(CImg<uint8_t> &src, int &N,
                         float alpha, float lvl)
{
    uint8_t *hash = (unsigned char*)malloc(72*sizeof(uint8_t));
    N = 72;

    CImg<uint8_t> img;

    if (src.spectrum() == 3){
        img = src.get_RGBtoYCbCr().channel(0).blur(1.0).resize(512,512,1,1,5).get_equalize(256);
    } else{
        img = src.channel(0).get_blur(1.0).resize(512,512,1,1,5).get_equalize(256);
    }
    src.clear();

    CImg<float> *pkernel = GetMHKernel(alpha,lvl);
    CImg<float> fresp =  img.get_correlate(*pkernel);
    img.clear();
    fresp.normalize(0,1.0);
    CImg<float> blocks(31,31,1,1,0);
    for (int rindex=0;rindex < 31;rindex++){
        for (int cindex=0;cindex < 31;cindex++){
            blocks(rindex,cindex) = (float)fresp.get_crop(rindex*16,cindex*16,rindex*16+16-1,cindex*16+16-1).sum();
        }
    }
    int hash_index;
    int nb_ones = 0, nb_zeros = 0;
    int bit_index = 0;
    unsigned char hashbyte = 0;
    for (int rindex=0;rindex < 31-2;rindex+=4){
        CImg<float> subsec;
        for (int cindex=0;cindex < 31-2;cindex+=4){
            subsec = blocks.get_crop(cindex,rindex, cindex+2, rindex+2).unroll('x');
            float ave = (float)subsec.mean();
            cimg_forX(subsec, I){
                hashbyte <<= 1;
                if (subsec(I) > ave){
                    hashbyte |= 0x01;
                    nb_ones++;
                } else {
                    nb_zeros++;
                }
                bit_index++;
                if ((bit_index%8) == 0){
                    hash_index = (int)(bit_index/8) - 1;
                    hash[hash_index] = hashbyte;
                    hashbyte = 0x00;
                }
            }
        }
    }

    return hash;
}

int ph_bitcount8(uint8_t val){
    int num = 0;
    while (val){
        ++num;
        val &= val - 1;
    }
    return num;
}

double ph_hammingdistance2(uint8_t *hashA, int lenA, uint8_t *hashB, int lenB){
    if (lenA != lenB){
        return -1.0;
    }
    if ((hashA == NULL) || (hashB == NULL) || (lenA <= 0)){
        return -1.0;
    }
    double dist = 0;
    uint8_t D = 0;
    for (int i=0;i<lenA;i++){
        D = hashA[i]^hashB[i];
        dist = dist + (double)ph_bitcount8(D);
    }
    double bits = (double)lenA*8;
    return dist/bits;

}
