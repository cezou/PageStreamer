# Squid Norme

La **Squid Norme** est un ensemble de règles simples et claires pour structurer 
et écrire des fichiers de manière cohérente, afin de faciliter la collaboration
et améliorer la lisibilité du code.

## Règles fondamentales

### 1. Limites de ligne
- Limitez les lignes à 80 caractères.

### 2. Commentaires
- Aucun commentaire à l'intérieur des fonctions, sauf potentiellement des 
  commentaires brefs de fin de ligne pour expliquer des détails complexes.
- Chaque fonction doit être documentée au format Doxygen avec une description 
  brève qui explique ce qu'elle fait, comment elle le fait, et détaille ses 
  arguments.
- La documentation doit être suffisante pour comprendre la fonction en 
  survolant uniquement son prototype depuis un IDE.
- Tous les commentaires doivent être écrits en anglais.

### 3. Espacement
- Entre deux fonctions, il doit y avoir exactement une ligne vide.
- À l'intérieur des fonctions, aucune ligne vide n'est autorisée, sauf après la liste de déclarations de variables, en début de fonction.

### 4. Structure
- Les déclarations de variables doivent être regroupées au début de la fonction.
- Une fonction doit idéalement accomplir une seule tâche clairement définie.

### 5. Nommage
- Les noms de variables et de fonctions doivent être en camelCase, et en anglais.
- Les noms doivent être descriptifs et refléter clairement leur usage.

## Exemple

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

En suivant ces règles, vous faciliterez la collaboration et la compréhension 
pour les humains et les IA.
