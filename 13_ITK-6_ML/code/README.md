# ITK-6: Machine Learning

This example shows how to use machine learning using ITK and OpenCV.

Compile ITK with the flag <b>Module_ITKVideoBridgeOpenCV</b> enabled.

# Usage

Windows:

```ITK_Tutorial_ML.exe --csvFile C:/Tutorials/13_ITK-5_ML/code/data/machine_learning/list.csv --images 'T1,T2,FL,PD,MANUAL,FOREGROUND' --saveFile C:/Tutorials/13_ITK-5_ML/code/data/machine_learning/trained.xml```

Linux/Mac:

```./ITK_Tutorial_ML --csvFile /home/Tutorials/13_ITK-5_ML/code/data/machine_learning/list.csv --images 'T1,T2,FL,PD,MANUAL,FOREGROUND' --saveFile /home/Tutorials/13_ITK-5_ML/code/data/machine_learning/trained.xml```

<b>NOTE</b>: Only 3D images are supported in this example