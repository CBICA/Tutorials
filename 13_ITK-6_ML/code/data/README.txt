
Sample dataset for WMLS implementation in ITK;

Contains preprocessed multi-modal (T1,T2,FL,PD) MR images of 10 subjects, and ground-truth lesion masks;

Algo 1:

 For each subject;
	Read all 4 modalities as float
	Read lesion mask 
	Read foreground mask
	Extract intensities of "lesion voxels" from 4 modalities
	Extract intensities of "non-lesion voxels" from 4 modalities
		! There are more non-lesion voxels than lesions
		  For now, select randomly a subset with size
		  equal to number of lesion voxels;	 
 End 

 Concat lesion intensities (from all subjects) into a matrix (size n x 4, n being total number of lesion voxels for all subjects)

 Concat non-lesion intensities into a matrix (size n x 4) 	

 Run training for the classifier (e.g. SVM)
