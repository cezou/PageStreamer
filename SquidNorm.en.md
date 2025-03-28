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
- Each function must be documented in Doxygen format with a brief description 
  that explains what it does, how it does it, and details its arguments.
- Documentation should be sufficient to understand the function by 
  hovering over its prototype from an IDE.
- All comments must be written in English.

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

```javascript
/**
 * @brief Processes data array and returns statistics
 * 
 * This function analyzes a number array, filters values based on a threshold,
 * calculates various statistics (mean, median, standard deviation)
 * and formats the result as JSON.
 * 
 * @param {number[]} dataArray - Array of numbers to analyze
 * @param {number} threshold - Minimum value to consider in analysis
 * @param {boolean} includeMean - If true, includes the mean in result
 * @param {boolean} includeMedian - If true, includes the median in result
 * @param {boolean} includeStdDev - If true, includes standard deviation
 * @return {object} JSON object containing requested statistics
 */
function processDataStatistics(dataArray, threshold = 0, includeMean = true,
                               includeMedian = true, includeStdDev = true) {
    let resultObject = {};
    let filteredData = [];
    let sum = 0;
    let meanValue = 0;
    let medianValue = 0;
    let standardDev = 0;
    let i = 0;
    let dataCount = 0;
    let variance = 0; // Variables are declared at the beginning
                      // Only one empty line allowed here
    filteredData = dataArray.filter(value => value >= threshold);
    dataCount = filteredData.length;
    if (dataCount === 0) {
        return { error: "No data exceeds the threshold" };
    }
    for (i = 0; i < dataCount; i++) {
        sum += filteredData[i];
    }
    meanValue = sum / dataCount;
    if (includeMean) {
        resultObject.mean = meanValue;
    }
    if (includeMedian) {
        filteredData.sort((a, b) => a - b);
        if (dataCount % 2 === 0) {
            medianValue = (filteredData[dataCount/2 - 1] + 
                          filteredData[dataCount/2]) / 2;
        } else {
            medianValue = filteredData[Math.floor(dataCount/2)];
        }
        resultObject.median = medianValue;
    }
    if (includeStdDev) {
        for (i = 0; i < dataCount; i++) {
            variance += Math.pow(filteredData[i] - meanValue, 2);
        }
        standardDev = Math.sqrt(variance / dataCount);
        resultObject.standardDeviation = standardDev;
    }
    return resultObject;
}
```

By following these rules, you will facilitate collaboration and understanding
for both humans and AI.
