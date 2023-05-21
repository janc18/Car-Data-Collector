package main

import (
	"fmt"
	"janc18/tui/formater"
)

func main() {

	File_path_name := "test-dataset.txt"

	data := formater.FormatDataSet(File_path_name)
	//randomgen.GenerateRandomDataset(5)

	fmt.Println(data[0])

}
