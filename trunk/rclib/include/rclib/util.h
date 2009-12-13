#ifndef __RCLIB__UTIL__H__INCLUDED__
#define __RCLIB__UTIL__H__INCLUDED__


#define DISABLE_COPY_SEMANTIC( CLASS_NAME )\
    private:\
        CLASS_NAME(const CLASS_NAME&);\
        CLASS_NAME& operator=(const CLASS_NAME&);
        
#endif
