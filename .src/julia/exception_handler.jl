#
# Copyright 2021 Clemens Cords
# Created on 26.12.2021 by clem (mail@clemens-cords.com)
#
begin # included into module jluna

    """
    offers verbose exception interface. Any call with safe_call will store
    the last exception and full stack trace as string in _last_exception and
    _last_message respectively
    """
    module exception_handler

        struct NoException <: Exception end
        export NoException

        mutable struct State
            _last_exception
            _last_message::String
        end

        const _state = Ref{State}(State(NoException(), ""));
        _meta_exception_message = ""

        """
        call any line of code, update the handler then forward the result, if any

        @param command: julia code as expression
        @returns result of expression or nothing if an exception was thrown
        """
        function safe_call(as_expression::Expr) ::Any

            result = undef
            try
                result = Main.eval(as_expression)
                update()
            catch exc
                result = nothing
                update(exc)
            end

            return result
        end

        """
        call any function, update the handler then forward the result, if any

        @param f: function
        @param args: function argument
        @returns result if function didn't throw, nothing otherwise
        """
        function safe_call(f::Function, args...)

            result = undef
            try
                result = f(args...)
                update()
            catch exc
                result = nothing
                update(exc)
            end

            return result;
        end

        """
        update the handler after an exception was thrown
        @param exception
        """
        function update(exception::Exception) ::Nothing

            try
            global _state[]._last_message = sprint(Base.showerror, exception, catch_backtrace())
            global _state[]._last_exception = exception
            catch e end
            return nothing
        end

        """
        update the handler after *no* exception was thrown
        """
        function update() ::Nothing

            global _state[]._last_message = ""
            global _state[]._last_exception = NoException()
            return nothing
        end

        """
        safe the current state of the handler

        @returns jluna.ExceptionHandler.State
        """
        function state() ::State

            return State(_last_exception, _last_message)
        end

        """
        is last exception type no "jluna.exception_handler.NoException"

        @returns bool
        """
        function has_exception_occurred() ::Bool

            return typeof(_state[]._last_exception) != NoException
        end

        """
        get last exception stacktrace

        @returns error message as string
        """
        function get_last_message() ::String
            return _state[]._last_message
        end

        """
        get last exception

        @returns exception
        """
        function get_last_exception() ::Exception
            return _state[]._last_exception
        end
    end
end
