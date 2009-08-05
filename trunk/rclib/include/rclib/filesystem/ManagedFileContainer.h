#ifndef ManagedFileContainer_h__
#define ManagedFileContainer_h__

#include "FileSlice.h"

#include <boost/filesystem.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <string>
#include <map>

namespace rclib
{
    namespace filesystem
    {
        template <typename CharT=char>
        class ManagedFileContainer
        {
        //static operations
        public:
            FileSlice<CharT> open(const std::basic_string<CharT> &filename, size_t start=0, size_t len=-1);
        protected:
        private:
            typedef std::map<const std::basic_string<CharT>, boost::interprocess::file_mapping*> CONTAINER;
            CONTAINER files;
        };

        template <typename CharT>
        FileSlice<CharT> ManagedFileContainer<CharT>::open( const std::basic_string<CharT> &filename, size_t start/*=0*/, size_t len/*=-1*/ )
        {
            assert(boost::filesystem::path(filename).is_complete());

            if (!boost::filesystem::exists(filename))
                throw;
            CONTAINER::iterator it;
            if ( (it = files.find(filename)) == files.end() )
            {
                //open the file
                //CONTAINER::value_type(filename, boost::interprocess::mapped_region(filename) )
                it = files.insert(it, 
                    CONTAINER::value_type(filename, 
                        new boost::interprocess::file_mapping(filename.c_str(), boost::interprocess::read_only)) );
            }
            return FileSlice<CharT>(this, it->second, start, len);
        }
    }
}

#endif // ManagedFileContainer_h__