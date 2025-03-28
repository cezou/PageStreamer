# Squid Norm

The **Squid Norm** is a set of simple and clear rules for structuring 
and writing files in a consistent manner, to facilitate collaboration 
and improve code readability.

## Fundamental Rules

### 1. Line Limits
- Limit lines to 80 characters.

### 2. Comments
- No comments inside functions, except potentially brief end-of-line 
  comments to explain complex details.
- All comments must be written in English.
- **Comment placement**:
  - Document classes with a Doxygen comment block before the class definition, 
    but not inside the class.
  - Document function/method implementations with a Doxygen comment block in 
    the implementation file (.cpp, .js, etc.), not in the header/interface file.
  - Do not duplicate documentation between declaration and implementation.
  - When documenting APIs, document the interface rather than the implementation.

### 3. Spacing
- Between two functions, there must be exactly one empty line.
- Inside functions, no empty lines are allowed, except after the list of 
  variable declarations at the beginning of the function.

### 4. Structure
- Variable declarations must be grouped at the beginning of the function.
- A function should ideally accomplish a single clearly defined task.

### 5. Naming
- Variables and function names must be in camelCase, and in English.
- Names should be descriptive and clearly reflect their usage.

## Example

### Header file (.hpp)

```cpp
/**
 * @brief Mathematical utility class
 * 
 * This class provides various mathematical operations.
 */
class MathUtils {
private:
    double precision;
    
public:
    MathUtils(double precision = 0.0001);
    ~MathUtils();
    
    double computeAverage(const std::vector<double>& values);
    bool isApproximatelyEqual(double a, double b);
};
```

### Implementation file (.cpp)

```cpp
/**
 * @brief Constructor
 * 
 * Initializes the MathUtils with the specified precision.
 * 
 * @param precision The precision value for approximate comparisons
 */
MathUtils::MathUtils(double precision) {
    this->precision = precision;
}

/**
 * @brief Computes the average of a set of values
 * 
 * @param values The vector of double values to average
 * @return The arithmetic mean of the values
 */
double MathUtils::computeAverage(const std::vector<double>& values) {
    double sum = 0.0;
    int count = values.size();  // Variables are declared at the beginning
                                // Only one empty line allowed here
    if (count == 0) {
        return 0.0;
    }
    for (int i = 0; i < count; i++) {
        sum += values[i];
    }
    return sum / count;
}
```

By following these rules, you will facilitate collaboration and understanding
for both humans and AI.

# Author

Cezou