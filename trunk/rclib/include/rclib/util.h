#pragma once



#define DISABLE_COPY_SEMANTIC( CLASS_NAME )\
    private:\
        CLASS_NAME(const CLASS_NAME&);\
        CLASS_NAME& operator=(const CLASS_NAME&);