---
trigger: model_decision
description: When creating unity tests
---

# Unit Testing Guide

This guide defines the standard approach for creating unit tests in this project using our custom test framework.

---

## Test Framework Overview

All unit tests must use the custom test framework located at `src/lib/test_framework/`.

### Key Components:
- **test_framework.h**: Header with test registration functions and assertion macros
- **test_framework.c**: Implementation of the test runner
- **Assertion Macros**: Predefined macros for common test assertions

---

## File Naming Convention

Test files must follow this naming pattern:
- **Pattern**: `<module_name>.spec.c`
- **Location**: Same directory as the module being tested
- **Example**: `src/lib/args_handler/args_handler.spec.c`

---

## Standard Test File Structure

Every test file must follow this structure:

```c
/**
 * @file <module_name>.spec.c
 * @brief Unit tests for <module_name> module
 *
 * Comprehensive unit tests for the functions defined in <module_name>.h
 */

#include "<module_name>.h"
#include "../test_framework/test_framework.h"

#include <string.h>  // Only if needed for string operations

// ============================================================================
// Tests for <function_name>()
// ============================================================================

/**
 * Test: <brief description of what is being tested>
 */
bool test_<function_name>_<scenario>(void) {
    // Arrange: Set up test data
    
    // Act: Call the function being tested
    
    // Assert: Verify the results
    ASSERT_<TYPE>(condition);
    
    return true;
}

// More tests...

// ============================================================================
// Main test runner
// ============================================================================

int main(void) {
    // Initialize test framework
    test_framework_init();
    
    // Register tests with sections
    test_print_section("Testing <function_name>()");
    test_register("test_<function_name>_<scenario>", test_<function_name>_<scenario>);
    // Register more tests...
    
    // Run all tests
    int result = test_run_all();
    
    // Cleanup
    test_framework_cleanup();
    
    return result;
}
```

---

## Test Function Guidelines

### 1. Function Signature
All test functions MUST:
- Return `bool`
- Take no parameters: `(void)`
- Return `true` at the end if all assertions pass

```c
bool test_example(void) {
    // Test code
    return true;
}
```

### 2. Naming Convention
Use descriptive names following this pattern:
- **Format**: `test_<function_name>_<scenario>`
- **Examples**:
  - `test_get_option_value_basic`
  - `test_get_option_value_not_found`
  - `test_circle_create_valid_input`
  - `test_list_insert_empty_list`

### 3. Test Organization
- Group tests by the function they test
- Use section comments (`// ===...===`) to separate groups
- Use `test_print_section()` to organize output

---

## Assertion Macros

Use these macros instead of manual if-statements:

| Macro | Usage | Description |
|-------|-------|-------------|
| `ASSERT_TRUE(cond)` | `ASSERT_TRUE(x > 0)` | Asserts condition is true |
| `ASSERT_FALSE(cond)` | `ASSERT_FALSE(is_empty)` | Asserts condition is false |
| `ASSERT_NULL(ptr)` | `ASSERT_NULL(result)` | Asserts pointer is NULL |
| `ASSERT_NOT_NULL(ptr)` | `ASSERT_NOT_NULL(obj)` | Asserts pointer is not NULL |
| `ASSERT_EQUAL(a, b)` | `ASSERT_EQUAL(count, 5)` | Asserts a == b |
| `ASSERT_NOT_EQUAL(a, b)` | `ASSERT_NOT_EQUAL(id, 0)` | Asserts a != b |
| `ASSERT_STR_EQUAL(s1, s2)` | `ASSERT_STR_EQUAL(name, "test")` | Asserts strings are equal |

### Assertion Behavior
- If assertion fails, the macro returns `false` immediately
- No need for manual return statements after assertions
- Always return `true` at the end of the test function

---

## Test Registration

### Basic Registration
```c
test_framework_init();
test_register("test_name", test_function);
int result = test_run_all();
test_framework_cleanup();
```

### With Sections (Recommended)
```c
test_framework_init();

test_print_section("Testing Module A");
test_register("test_a_scenario1", test_a_scenario1);
test_register("test_a_scenario2", test_a_scenario2);

test_print_section("Testing Module B");
test_register("test_b_scenario1", test_b_scenario1);

int result = test_run_all();
test_framework_cleanup();
```

---

## Compilation Instructions

### Standard Compilation Command
```bash
gcc -std=c99 -fstack-protector-all \
    -I../test_framework \
    -o <module>_test \
    <module>.spec.c \
    <module>.c \
    ../test_framework/test_framework.c
```

### Example
```bash
cd src/lib/args_handler
gcc -std=c99 -fstack-protector-all \
    -I../test_framework \
    -o args_handler_test \
    args_handler.spec.c \
    args_handler.c \
    ../test_framework/test_framework.c
```

### Running Tests
```bash
./<module>_test
```

---

## Test Coverage Requirements

For each public function in a module, create tests for:

1. **Happy Path**: Normal, expected usage
2. **Edge Cases**: Boundary conditions (empty, zero, max values)
3. **Error Cases**: Invalid input, NULL pointers
4. **Multiple Scenarios**: Different valid input combinations

### Example Coverage for `get_option_value()`:
- ✅ Basic usage with valid option
- ✅ Option not found
- ✅ Option without value
- ✅ Empty arguments
- ✅ Multiple options
- ✅ Special characters in values

---

## Best Practices

### DO:
✅ Use descriptive test names  
✅ Test one behavior per test function  
✅ Use assertion macros instead of manual checks  
✅ Group related tests with section headers  
✅ Return `true` at the end of successful tests  
✅ Test edge cases and error conditions  
✅ Keep tests independent (no shared state)  

### DON'T:
❌ Test multiple unrelated behaviors in one test  
❌ Use manual if-statements instead of assertion macros  
❌ Forget to call `test_framework_init()` and `test_framework_cleanup()`  
❌ Test private/internal functions (test through public API)  
❌ Create tests that depend on execution order  
❌ Ignore edge cases and error conditions  

---

## Example: Complete Test File

See the reference implementation at:
- **File**: `src/lib/args_handler/args_handler.spec.c`
- **Module**: `args_handler`
- **Coverage**: 13 tests covering all public functions

This example demonstrates:
- Proper file structure
- Test organization by function
- Use of all assertion macros
- Section headers for clarity
- Comprehensive coverage

---

## Troubleshooting

### Common Issues:

**Issue**: Test compiles but doesn't run  
**Solution**: Ensure `test_framework_init()` is called before registering tests

**Issue**: Assertion fails but test continues  
**Solution**: Make sure you're using the assertion macros, not manual if-statements

**Issue**: "Maximum number of tests exceeded"  
**Solution**: Increase `MAX_TESTS` in `test_framework.c` (default: 100)

**Issue**: Segmentation fault in tests  
**Solution**: Check for NULL pointer dereferences; use `ASSERT_NOT_NULL()` before accessing pointers

---

## Summary Checklist

When creating a new test file, ensure:

- [ ] File named `<module>.spec.c` in same directory as module
- [ ] Includes `test_framework.h` and module header
- [ ] Test functions return `bool` and take `(void)`
- [ ] Uses assertion macros (not manual checks)
- [ ] Tests grouped by function with section comments
- [ ] `main()` initializes framework, registers tests, runs, and cleans up
- [ ] Descriptive test names following `test_<function>_<scenario>` pattern
- [ ] Covers happy path, edge cases, and error conditions
- [ ] Compiles with `-std=c99 -fstack-protector-all`
- [ ] All tests pass before committing
