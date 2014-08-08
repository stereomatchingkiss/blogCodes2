#include "example00.hpp"

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

#include <cstdlib> //std::system
#include <functional>
#include <iostream>
#include <sstream>

namespace{

void construct_objects(boost::interprocess::managed_shared_memory &managed_shm)
{
    managed_shm.construct<int>("Integer")(99);
    managed_shm.construct<float>("Float")(3.14);
}


struct shm_remove
{
public:
    shm_remove(std::string const &name) : name_(name)
    {
        boost::interprocess::shared_memory_object::remove(name_.c_str());
    }
    ~shm_remove(){  boost::interprocess::shared_memory_object::remove(name_.c_str()); }

private:
    std::string name_;
};

}

void atomic_func_00()
{
    using namespace boost::interprocess;

    shm_remove remover("MySharedMemory");

    shared_memory_object::remove("Highscore");
    managed_shared_memory managed_shm(boost::interprocess::open_or_create,
                                      "Highscore",
                                      1024);
    auto func = std::bind(construct_objects, std::ref(managed_shm));
    managed_shm.atomic_func(func);
    std::cout << *managed_shm.find<int>("Integer").first << std::endl;
    std::cout << *managed_shm.find<float>("Float").first << std::endl;
}

int example_00(int argc, char const* argv[])
{
    using namespace boost::interprocess;
    if(argc == 1){  //Parent process
        //Remove shared memory on construction and destruction
        shm_remove remover("MySharedMemory");

        //Create a managed shared memory segment
        managed_shared_memory segment(create_only, "MySharedMemory", 65536);

        //Allocate a portion of the segment (raw memory)
        managed_shared_memory::size_type free_memory = segment.get_free_memory();
        void *shptr = segment.allocate(1024/*bytes to allocate*/);

        //Check invariant
        if(free_memory <= segment.get_free_memory()){
            //return 1;
            std::cerr<<"free_memory <= segment.get_free_memory()"<<std::endl;
        }

        //An handle from the base address can identify any byte of the shared
        //memory segment even if it is mapped in different base addresses
        managed_shared_memory::handle_t handle = segment.get_handle_from_address(shptr);
        std::stringstream s;
        s << argv[0] << " " << handle;
        s << std::ends;
        //Launch child process
        if(0 != std::system(s.str().c_str())){
            std::cerr<<"0 != std::system(s.str().c_str())"<<std::endl;
            return 1;
        }
        //Check memory has been freed
        if(free_memory != segment.get_free_memory()){
            std::cerr<<"free_memory != segment.get_free_memory()"<<std::endl;
            return 1;
        }
    }else{
        shm_remove remover("MySharedMemory");
        //Open managed segment
        managed_shared_memory segment(open_only, "MySharedMemory");

        //An handle from the base address can identify any byte of the shared
        //memory segment even if it is mapped in different base addresses
        managed_shared_memory::handle_t handle = 0;

        //Obtain handle value
        std::stringstream s; s << argv[1]; s >> handle;

        //Get buffer local address from handle
        void *msg = segment.get_address_from_handle(handle);

        //Deallocate previously allocated memory
        segment.deallocate(msg);
    }

    return 0;
}


void example_01()
{
    shm_remove remover("Highscore");
    boost::interprocess::shared_memory_object shdmem(boost::interprocess::open_or_create,
                                                     "Highscore",
                                                     boost::interprocess::read_write);
    shdmem.truncate(1024);
    std::cout<<shdmem.get_name()<<std::endl;
    boost::interprocess::offset_t size;
    if (shdmem.get_size(size)){
        std::cout<<size<<std::endl;
    }
}

void mapped_region_00()
{
    shm_remove remover("Highscore");
    boost::interprocess::shared_memory_object shdmem(boost::interprocess::open_or_create,
                                                     "Highscore",
                                                     boost::interprocess::read_write);
    shdmem.truncate(1024);

    boost::interprocess::mapped_region region(shdmem, boost::interprocess::read_write);
    std::cout << std::hex << "0x" << region.get_address() << std::endl;
    std::cout << std::dec << region.get_size() << std::endl;

    boost::interprocess::mapped_region region2(shdmem, boost::interprocess::read_only);
    std::cout << std::hex << "0x" << region2.get_address() << std::endl;
    std::cout << std::dec << region2.get_size() << std::endl;
}


void mapped_memory_read_and_write_00()
{
    shm_remove remover("Highscore");
    boost::interprocess::shared_memory_object shdmem(boost::interprocess::open_or_create, "Highscore", boost::interprocess::read_write);
    shdmem.truncate(1024);

    boost::interprocess::mapped_region region(shdmem, boost::interprocess::read_write);
    int *i1 = static_cast<int*>(region.get_address());
    *i1 = 99;

    boost::interprocess::mapped_region region2(shdmem, boost::interprocess::read_only);
    int *i2 = static_cast<int*>(region2.get_address());
    std::cout << *i2 << std::endl;
}


void managed_shared_memory_00()
{
    shm_remove remover("Highscore");
    boost::interprocess::managed_shared_memory managed_shm(boost::interprocess::open_or_create, "Highscore", 1024);
    int *i = managed_shm.construct<int>("Integer")(99);
    std::cout << *i << std::endl;
    std::pair<int*, std::size_t> p = managed_shm.find<int>("Integer");
    if (p.first){
        std::cout << *p.first << std::endl;
    }
}


void managed_shared_memory_01()
{
    shm_remove remover("Highscore");
    boost::interprocess::managed_shared_memory managed_shm(boost::interprocess::open_or_create,
                                                           "Highscore",
                                                           1024);
    int *i = managed_shm.construct<int>("Integer")[10](99);
    std::cout << *i << std::endl;
    std::pair<int*, std::size_t> p = managed_shm.find<int>("Integer");
    if (p.first)
    {
        std::cout << *p.first << std::endl;
        std::cout << p.second << std::endl;
    }
}


void boost_interprocess_string()
{
    shm_remove remover("Highscore");

    boost::interprocess::shared_memory_object::remove("Highscore");
    boost::interprocess::managed_shared_memory managed_shm(boost::interprocess::open_or_create, "Highscore", 1024);
    typedef boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager> CharAllocator;
    typedef boost::interprocess::basic_string<char, std::char_traits<char>, CharAllocator> string;
    string *s = managed_shm.find_or_construct<string>("String")("Hello!", managed_shm.get_segment_manager());
    s->insert(5, ", world");
    std::cout << *s << std::endl;
}
