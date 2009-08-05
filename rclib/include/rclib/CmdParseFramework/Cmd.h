#pragma once

#include <functional>
#include <string>

namespace rclib
{
    template <typename Args>
    class Cmd
    {
    public:
		typedef std::tr1::function<Args> FuncType;

        Cmd(const std::wstring& cmdstring
            , size_t abbr
            , const std::wstring& description
            , FuncType cmdfunc)
        : _cmd(cmdstring), _abbr_len(abbr), _func(cmdfunc), _descrp(description)
        {
            if (_abbr_len == 0)
                _abbr_len = _cmd.length();
        }

        Cmd(const std::wstring& cmdstring
            , const std::wstring& abbrcmd
            , const std::wstring& description
            , FuncType cmdfunc)
        : _cmd(cmdstring), _abbr_cmd(abbrcmd), _abbr_len(0), _func(cmdfunc), _descrp(description)
        {}

        FuncType SetCmdFunc(FuncType cmdfunc)
        {
            cmdfunc.swap(_func);
            return cmdfunc;
        }

    public:
        bool CanExecute(const std::wstring& cmdline)
        {
            return false;
        }

        bool Execute(const Args& args)
        {
            return false;
        }
    private:
        std::wstring _cmd, _abbr_cmd, _descrp;
        size_t _abbr_len;
        FuncType _func;
    };
}

