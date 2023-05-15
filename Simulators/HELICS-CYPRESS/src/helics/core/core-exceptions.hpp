/*
Copyright (c) 2017-2023,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance for Sustainable
Energy, LLC.  See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once
#include <stdexcept>
#include <string>
#include <string_view>

/** @file
@details definitions of exceptions used in HELICS
*/

namespace helics {
/** base exception class for helics*/
class HelicsException: public std::exception {
  private:
    std::string errorMessage = "HELICS EXCEPTION";

  public:
    HelicsException() = default;
    explicit HelicsException(std::string_view message) noexcept: errorMessage(message) {}
    virtual const char* what() const noexcept override { return errorMessage.c_str(); }
};

/** exception class indicating that a function has failed for some reason*/
class FunctionExecutionFailure: public HelicsException {
  public:
    explicit FunctionExecutionFailure(
        std::string_view message = "HELICS execution failure") noexcept:
        HelicsException(message)
    {
    }
};

/** exception for an invalid identification Handle*/
class InvalidIdentifier: public HelicsException {
  public:
    explicit InvalidIdentifier(std::string_view message = "invalid identifier") noexcept:
        HelicsException(message)
    {
    }
};

/** exception when one or more of the parameters in the function call were invalid*/
class InvalidParameter: public HelicsException {
  public:
    explicit InvalidParameter(std::string_view message = "invalid parameter") noexcept:
        HelicsException(message)
    {
    }
};

/** exception when a requested conversion is unable to be performed*/
class InvalidConversion: public HelicsException {
  public:
    explicit InvalidConversion(
        std::string_view message = "unable to perform the requested conversion") noexcept:
        HelicsException(message)
    {
    }
};

/** exception thrown when a function call was made at an inappropriate time*/
class InvalidFunctionCall: public HelicsException {
  public:
    explicit InvalidFunctionCall(std::string_view message = "invalid function call") noexcept:
        HelicsException(message)
    {
    }
};

/** exception indicating that the connections of an object or network have failed*/
class ConnectionFailure: public HelicsException {
  public:
    explicit ConnectionFailure(std::string_view message = "failed to connect") noexcept:
        HelicsException(message)
    {
    }
};

/** exception indicating that the registration of an object has failed*/
class RegistrationFailure: public HelicsException {
  public:
    explicit RegistrationFailure(std::string_view message = "registration failure") noexcept:
        HelicsException(message)
    {
    }
};

/** severe exception indicating HELICS has failed and terminated unexpectedly*/
class HelicsSystemFailure: public HelicsException {
  public:
    explicit HelicsSystemFailure(std::string_view message = "HELICS system failure") noexcept:
        HelicsException(message)
    {
    }
};

/** exception generated by a HELICS federate*/
class FederateError: public HelicsException {
  public:
    int code{0};
    FederateError(int errorCode, std::string_view message) noexcept:
        HelicsException(message), code(errorCode)
    {
    }
};
}  // namespace helics