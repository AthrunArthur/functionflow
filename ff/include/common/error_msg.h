/***********************************************
The MIT License (MIT)

Copyright (c) 2012 Athrun Arthur <athrunarthur@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*************************************************/

//!This file is used to define compile-time error messages.
#ifndef FF_COMMON_ERROR_MSG_H_
#define FF_COMMON_ERROR_MSG_H_

#define FF_EM_CALL_THEN_WITHOUT_CALL_PAREN "You can only call *then* after operater(...)"

#define FF_EM_CALL_WITH_TYPE_MISMATCH "The function's return type in operatoer(...) doesn't match para<...>"

#define FF_EM_THEN_WITH_TYPE_MISMATCH "The parameter's type in then's callback function doesn't match associated para<...>'s return type"

#define FF_EM_THEN_WITH_NON_FUNC_TYPE "*then* must take a callable object as parameter, like lambda, functor, or std::function"

#define FF_EM_CALL_SQPAREN_AFTER_PAREN "Cannot using operator[] to wait for others!"

#define FF_EM_WRONG_USE_SQPAREN "You can only wait for a para<...> object or a dependency expression while using operator[]"

#define FF_EM_CALL_PAREN_AFTER_PAREN "You cannot call operator() after operator()"

#define FF_EM_COMBINE_PARA_AND_OTHER "Cannot combine para<...> object and other object as dependency expression"

#define FF_EM_USE_PARACONTAINER_INSTEAD_OF_GROUP "paragroup is only for data parallelism (for_each), use paracontainer to hold multiple para<...> objects!"

#define FF_EM_THEN_FUNC_TYPE_MISMATCH "The function's type in *then* doesn't match with associated dependency expression"

#endif
