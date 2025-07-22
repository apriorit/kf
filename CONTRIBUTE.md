# Code style
It includes entities/variables naming, brackets, tabulation, empty lines where they needed. Generally, the rules of code style are:

**Tabulation** is made by 4 spaces.

**Empty lines** before the entity declaration and before private/protected/public sections, also empty lines are used to separate code to logical sections.

**Curly braces** are placed in new lines like this:
```
function()
{
    code
}
 
switch
{
case1:
{
    code
}
default:
    1-line code
 
if (condition)
{
    code
}
```

# Naming
**The name of a file** should have the same name as the entity that is defined in it. For example, the file with class TreeSet should have a name TreeSet.h. 

**The names of entities** are in PascalCase, the names of functions, variables and data members are in camelCase, the names of private members of a class have m_ prefix, public constants have k prefix:
```
namespace kf
{
    constexpr int kPublicConstant = 1;
 
    struct Struct
    {
        int field1 = 0;
        char field2 = 0;
    };
 
    class Class
    {
    public:
        void set(Struct var)
        {
            if (var.field1 != kPublicConstant)
            {
                m_privateVar = var;
            }
        }
 
    private:
        Struct m_privateVar;
    };
}
```

# Comments
**Every class should have a general comment** above the definition with short explanation: what is it, for what purpose it serves, the general principle of its work.

**Inside the function, there should be no comments** except the case of optimized code, that is hard to understand. In any other cases, when you need a comment, change the code so that it doesn't need comments.

**TODO and FIX comments should contain the link** to the issue in Kernel Foundation project (https://jira.dev.local/jira/projects/KF). If there is no issue, ask DM to create it.

The commits with **commented code are forbidden**: remove or uncomment code before commit. 

# Warnings analysis
Before making PR do the warnings check on the projects that are modified: RMB on the project → Analyze and Code Cleanup → Run Code Analysis on <project_name>. Then fix all warnings.

# SAL annotations
Add SAL annotation to every created function. Also use these recomendations from MS: https://learn.microsoft.com/en-us/cpp/code-quality/best-practices-and-examples-sal?view=msvc-170.

# Checks
Use ASSERT  macro to check programmer's mistakes.

Check that IRQL is correct in every function/method that calls system function directly.

# Testing
Unit tests are mandatory, developer should not add/change functionality without adding/changing tests for it. We are using kmtest for this purpose, please read its documentation and existing tests for example.