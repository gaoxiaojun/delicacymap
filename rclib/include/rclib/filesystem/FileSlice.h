#ifndef FileSlice_h__
#define FileSlice_h__

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <memory>

namespace rclib
{
    namespace filesystem
    {
        template <typename CharT>
        class ManagedFileContainer;

        template <typename CharT=char>
        class FileSlice
        {
        public:
            const unsigned char* data() const {return reinterpret_cast<const unsigned char*>( region->get_address() );}
            size_t size() const {return region->get_size();}
        private:
            friend class ManagedFileContainer<CharT>;
            FileSlice(ManagedFileContainer<CharT>* manager, boost::interprocess::file_mapping *file, size_t start, size_t len)
                : parentcont(manager), region(new boost::interprocess::mapped_region(*file, boost::interprocess::read_only, start, len))
            {};
            void operator=(const FileSlice&);
            
            ManagedFileContainer<CharT> *parentcont;
            std::auto_ptr<boost::interprocess::mapped_region> region;
        };
    }
}

#endif // FileSlice_h__
