#pragma once
#include "chaiscript/chaiscript.hpp"

namespace engine
{
    namespace scripting
    {
        class CScript
        {
        public:
            CScript() = default;
            CScript(const std::string &program)
            {
                // Wrapping curly brackets to create a local area
                srProgramm = "{\n" + program + "\n}";
            }

            // Bind new property to script object
            template <class... _Args>
            void bind(_Args... args)
            {
                executor.add(std::forward<_Args>(args)...);
            }

            // Bind new global property to script object
            template <class... _Args>
            void bind_global(_Args... args)
            {
                executor.add_global(std::forward<_Args>(args)...);
            }

            // Bind namespace property
            template <class... _Args>
            void bind_namespace(_Args... args)
            {
                executor.register_namespace(std::forward<_Args>(args)...);
            }

            // Execute script and get last evaluation
            template <class _Ty>
            _Ty execute()
            {
                try
                {
                    // We save the state of the script before execution in order to recover with each new iteration
                    clearState = executor.get_state();
                    auto result = executor.eval<_Ty>(srProgramm);
                    // And restore it after execution
                    executor.set_state(clearState);
                    return result;
                }
                catch (const chaiscript::exception::eval_error &e)
                {
                    std::cerr << "In file: " << e.filename << " " << e.what() << std::endl;
                    throw e;
                }
            }

            // Execute script and get last evaluation
            template <>
            chaiscript::Boxed_Value execute<chaiscript::Boxed_Value>()
            {
                // We save the state of the script before execution in order to recover with each new iteration
                clearState = executor.get_state();
                try
                {
                    // We save the state of the script before execution in order to recover with each new iteration
                    clearState = executor.get_state();
                    auto result = executor.eval(srProgramm);
                    // And restore it after execution
                    executor.set_state(clearState);
                    return result;
                }
                catch (const chaiscript::exception::eval_error &e)
                {
                    std::cerr << "In file: " << e.filename << " " << e.what() << std::endl;
                    throw e;
                }
            }

            // Execute script and resolve result
            template <>
            void execute<void>()
            {
                auto result = execute<chaiscript::Boxed_Value>();
                resolve(result);
                // TODO: do something here
            }

        private:
            int32_t resolve(chaiscript::Boxed_Value &boxedValue);

        private:
            std::string srProgramm;
            chaiscript::ChaiScript executor;
            chaiscript::ChaiScript_Basic::State clearState;
        };
    }
}