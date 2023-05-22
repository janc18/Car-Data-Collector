package main

import (
	"flag"
	"fmt"
	"janc18/tui/formater"
	"janc18/tui/randomgen"
)

func main() {
	var datasetPath string = "test-dataset.txt"
	var generateRandomData bool

	flag.StringVar(&datasetPath, "f", "test-dataset.txt", "File path")
	flag.BoolVar(&generateRandomData, "g", false, "Generate random dataset")
	flag.Parse()

	if generateRandomData {
		randomgen.GenerateRandomDataset(30)
	}
	data := formater.FormatDataSet(datasetPath)

	fmt.Println(data)

}
