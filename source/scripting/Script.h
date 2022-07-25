#pragma once
#include "chaiscript/chaiscript.hpp"

namespace engine
{
    namespace scripting
    {
        class CScript
        {
        public:
            friend class CScriptBuilder;
            CScript() = default;
            ~CScript();

            // Bind new property to script object
            template<class... _Args>
            void bind(_Args... args)
            { executor.add(std::forward<_Args>(args)...); }

            // Bind new global property to script object
            template<class... _Args>
            void bind_global(_Args... args)
            { executor.add_global(std::forward<_Args>(args)...); }

            // Bind namespace property
            template<class... _Args>
            void bind_namespace(_Args... args)
            { executor.register_namespace(std::forward<_Args>(args)...); }

            template<class _Ty>
            _Ty call(const std::string& entryPoint = "main()")
            {
                if (bIsFirstCall)
                {
                    for(auto& [fname, code] : vIncludes)
                        execute<void>(code, fname);

                    bIsFirstCall = false;
                }
                
                auto entryPath = vIncludes.begin()->first;
                auto result = execute<_Ty>(entryPoint, entryPath);
                return result;
            }

            template<>
            void call(const std::string& entryPoint)
            {
                call<chaiscript::Boxed_Value>(entryPoint);
            }

        protected:
            // Execute script and get last evaluation
            template<class _Ty>
            _Ty execute(const std::string& entryPoint, const std::string& filename)
            {
                static_assert(!std::is_same_v<_Ty, void>, "void is not supported for execution. Use chaiscript::Boxed_Value.");
                try
                {
                    if constexpr (std::is_same_v<_Ty, chaiscript::Boxed_Value>)
                        return executor.eval(entryPoint, chaiscript::Exception_Handler(), filename);
                    else
                        return executor.eval<_Ty>(entryPoint, chaiscript::Exception_Handler(), filename);
                }
                catch (const chaiscript::exception::eval_error& e)
                {
                    std::stringstream ss;
                    ss << "In file: " << e.filename << " | " << e.what() << !e.detail.empty() ? " | " + e.detail : "";
        #ifdef _WIN32
                    OutputDebugString(ss.str().c_str());
        #endif
                    std::cerr << ss.str() << std::endl;
                    throw e;
                }
            }

            template<>
            void execute(const std::string& entryPoint, const std::string& filename)
            {
                execute<chaiscript::Boxed_Value>(entryPoint, filename);
            }
        private:
            int32_t resolve(chaiscript::Boxed_Value& boxedValue);
            chaiscript::ChaiScript executor;
            chaiscript::ChaiScript_Basic::State lastState;
            std::vector<std::pair<std::string, std::string>> vIncludes;
            bool bIsFirstCall{ true };
        };
    }
}