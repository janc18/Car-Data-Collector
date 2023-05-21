package randomgen

import (
	"fmt"
	"log"
	"math/rand"
	"os"
)

func GenerateRandomDataset(iterations int) {
	File_path_name := "test-dataset.txt"

	f, err := os.Create(File_path_name)
	if err != nil {
		log.Fatal(err)
	}
	defer f.Close()
	var i int
	for i = 0; i <= iterations; i++ {
		accelData := fmt.Sprintf("%d|%d|%d|", rand.Intn(65535), rand.Intn(65535), rand.Intn(65535))
		giroData := fmt.Sprintf("%d|%d|%d|\n", rand.Intn(65535), rand.Intn(65535), rand.Intn(65535))
		_, err := f.WriteString(accelData + giroData)
		if err != nil {
			log.Fatal(nil)
		}
	}
}
