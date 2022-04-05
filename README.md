## About
simple_library - written for wednesday.wtf
- libraries for simple / complicated tasks.

### Dependencies
- None at the moment.

### Notes
> If you have issues using clang-format, download `Clang Power Tools` and `Clang Format` in the VS2019 extension manager.
> If you are unable to compile with C++23 change it down to C++20. There may be some backend work you'll have to put in.

### Code Style
```c++
/* ~Forward Definitions~ */
class c_base_class; => struct c_base_class;
class my_class; => struct my_struct;
/* ~Forward Definitions~ */

/* ~Structs */
struct my_struct {
  private:
    int my_private_int;

  public:
    int my_public_int;
 
  protected:
    int my_protected_int;
}
/* ~Structs */

/* ~Functions~ */
void my_function( int my_arg, int my_arg );

template< typename T >
void my_template_function( T /* OR C++23 auto */ arg );

my_template_function< int >( 23 );
/* ~Functions~ */

/* ~Other~ */
// Strings arguments should always be, string_view, and C strings (char* with null terminator).
// Always use modern c++ casting such as, reinterpret, static, and const.
/* ~Other~ */
```
