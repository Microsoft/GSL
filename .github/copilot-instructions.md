# GitHub Copilot Instructions for GSL (Guidelines Support Library)

## Project Overview
This repository contains the Guidelines Support Library (GSL), a Microsoft implementation of types and functions 
suggested for use by the C++ Core Guidelines. It's a header-only C++ library with emphasis on safety, 
correctness, and zero overhead.

## Coding Standards

### General
- Follow C++ Core Guidelines wherever possible
- Use meaningful type, function, and template parameter names
- Keep functions small and focused with clear preconditions/postconditions
- Include comments for complex code, but prefer self-documenting code
- Use the Expects() and Ensures() macros for contract verification

### Style Guidelines
- Use 4 spaces for indentation (not tabs)
- Maximum line length of 100 characters
- Follow GSL naming conventions (lowercase with underscores)
- Keep templates clean and readable with appropriate spacing
- Use C++14 features since this is the minimum standard supported

### Error Handling
- Use Expects() for preconditions and Ensures() for postconditions
- Design for fail-fast semantics (std::terminate) on contract violations
- Template constraints should use static_assert or SFINAE
- Don't throw exceptions from basic operations

### Testing
- Write thorough unit tests for every component using GTest
- Test for all edge cases and error conditions
- Ensure cross-platform compatibility in tests
- Maintain 100% code coverage for changed code

## Project-Specific Conventions

### Architecture
- All public types must be in the gsl namespace
- Design for zero overhead abstractions when possible
- Respect the distinction between Owners and Views
- Maintain backward compatibility with existing GSL code

### Version Control
- Link all PRs to related issues
- Use clear commit messages explaining what and why
- Follow the contribution guidelines documented in CONTRIBUTING.md
- PRs should include appropriate tests with 100% coverage for changed code

### Documentation
- Document all public APIs with clarity on preconditions and postconditions
- Keep header comments up-to-date
- Include examples for complex functionality in docs/headers.md

## Technology Stack
- C++14 (minimum) for core implementation
- CMake build system (3.14+)
- Google Test for unit testing
- Support for multiple compilers (MSVC, GCC, Clang)

## Security Considerations
- Bounds checking is a core principle - enforce it consistently
- Design for safety while minimizing overhead
- Ensure undefined behavior is explicitly detected where possible

## Performance Guidelines
- Optimize for both safety and performance
- Constexpr-enable functions wherever possible
- Avoid hidden allocations
- Use noexcept appropriately for move operations and other performance-critical functions

## Cross-Platform Support
- Code must work across:
  - Windows (MSVC)
  - Linux (GCC, Clang)
  - macOS (AppleClang)

## Copilot Tasks
- Configure, build, and test C++14 with:
  - `cmake --preset clang-14-debug -DCI_TESTING:BOOL=ON`
  - `cmake --build --preset clang-14-debug`
  - `ctest --preset clang-14-debug --output-on-failure`
- Configure, build, and test C++20 with:
  - `cmake --preset clang-20-debug -DCI_TESTING:BOOL=ON`
  - `cmake --build --preset clang-20-debug`
  - `ctest --preset clang-20-debug --output-on-failure`
- Agent-created RCA pull requests must contain a failing regression test but no production fix and
  use `Status: RCA Complete`.
- Coding agents must continue on the existing RCA pull request and must not weaken its regression
  test. They move validated pull requests to `Status: Under Review`.
- Never perform autonomous work while `Agent: Paused` is present. Use `Human Decision Required`
  when a design, compatibility, or policy decision requires a maintainer.
- Agent reviews are non-approving first-pass reviews. They classify pull requests by priority, kind,
  risk, size, and scope before requesting review from `@carsonradtke`.
- Before publishing a PR, verify the following:
  - There are no compiler warnings or errors when building the test suite.
  - The test suite passes on all supported platforms and compilers.
  - The test suite passes for both C++14 and C++20.
