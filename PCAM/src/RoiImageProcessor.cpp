#include "RoiImageProcessor.h"

RoiImageProcessor::RoiImageProcessor(AbstractConfiguration * cfg) {
	this->cfg = cfg;
    
    networkImageIndex = 0;

    procMethod = cfg->getString("Processing.Method","canny");
    maxQueueSize = cfg->getInt("Processing.MaxQueueSize",60);
    downsampleFactor = cfg->getInt("Processing.DownsampleFactor",4);
    roiPad = cfg->getInt("Processing.ROIPad",4);
    bwThreshold = cfg->getInt("Processing.Threshold",20);
    sel7Size = cfg->getInt("Processing.Sel7Size",7);
    sel3Size = cfg->getInt("Processing.Sel3Size",3);
    laplaceSize = cfg->getInt("Processing.LaplaceSize",1);
    cannyLow = cfg->getInt("Processing.CannyLow",50);
    cannyHigh = cfg->getInt("Processing.CannyHigh",100);
    cannyKernelSize = cfg->getInt("Processing.CannyKernelSize",3);
	minContourArea = cfg->getDouble("Processing.MinContourArea",36);
    minArea = cfg->getDouble("Processing.MinArea",15*15);
    maxArea = cfg->getDouble("Processing.MaxArea",500*500);
	minMal = cfg->getDouble("Processing.MinMal",10);
	maxMal = cfg->getDouble("Processing.MaxMal",4000);
    maxRoisPerImage = cfg->getInt("Processing.MaxROISPerImage",50);
	binningX = cfg->getInt("Camera.HorzBinning",1);
	binningY = cfg->getInt("Camera.VertBinning",1);
	rawImgWidth = cfg->getInt("Camera.ImageWidth",3376)/binningX;
	rawImgHeight = cfg->getInt("Camera.ImageHeight",2704)/binningY;
	if (cfg->getString("Camera.PixelFormat","BayerRG16") == "BayerRG16")
		pixelFormat = 2;
	else
		pixelFormat = 1;
	
	cameraName = cfg->getString("App.CameraName","SPC");
	checkArea = cfg->getBool("Processing.CheckArea",true);
	checkMal = cfg->getBool("Processing.CheckMal",true);

    saveShrink = cfg->getBool("App.SaveShrinkFiles",true);
    shrinkThreshold = cfg->getInt("Processing.ShrinkThreshold",35);
    shrinkSubSample = cfg->getInt("Processing.ShrinkSubSample",2);
	
    networkDir = cfg->getString("Processing.NetworkDir","/home/spc/network_images");
    saveNetworkImage = cfg->getBool("Processing.SaveNetworkImage",true);
    numNetworkImages = cfg->getInt("Processing.NumNetworkImages",10);
    networkSubSample = cfg->getInt("Processing.ShrinkSubSample",4);

    // Vectors for contour finding in images
    sel7 = getStructuringElement(MORPH_RECT,Size(sel7Size,sel7Size),Point(-1,-1));
    sel3 = getStructuringElement(MORPH_RECT,Size(sel3Size,sel3Size),Point(-1,-1));

    // Allocate images based on raw and downsample sizes
    dsWidth = rawImgWidth/downsampleFactor;
    dsHeight = rawImgHeight/downsampleFactor;

    // // 16 bit images for filtering
	if (pixelFormat == 2) {
		dsImg = Mat( dsWidth, dsHeight, CV_16UC1);
		thImg = Mat( dsWidth, dsHeight, CV_16UC1);
	
	}
	else{
		dsImg = Mat( dsWidth, dsHeight, CV_8UC1);
		thImg = Mat( dsWidth, dsHeight, CV_8UC1);
	}

    // 8-bit images for binary ops
    thImg1 = Mat( dsWidth, dsHeight, CV_8UC1);
    bw1 = Mat( dsWidth, dsHeight, CV_8UC1);
    bw2 = Mat( dsWidth, dsHeight, CV_8UC1);
    subSampleImg = Mat( dsWidth/shrinkSubSample, dsHeight/shrinkSubSample, CV_8UC1);
    subSampleThresh = Mat( dsWidth/shrinkSubSample, dsHeight/shrinkSubSample, CV_8UC1);

    // Misc variables for processing and saving rois
    logTimer = 0;
	
	totalRois = 0;
	recentRoiCount = 0;
	
	// Set busy flag to flase
	busy = false;
	
	threadDelay = 10000;
	

}

RoiImageProcessor::~RoiImageProcessor(void)
{

}

void RoiImageProcessor::updateShrinkDir(string shrinkDir) {
    lock();
    this->shrinkDir = shrinkDir;
    unlock();
}

void RoiImageProcessor::updateDataDir(string dataDir) {
	lock();
	this-> dataDir = dataDir;
	unlock();
}

void RoiImageProcessor::processDataEntry(DataEntry* entry) {


	lock();
	busy = true;
	unlock();
	
	// Cast entry to cameraImage
	CameraImage * pImage = (CameraImage*)entry;
	

	
	if (pImage->height != rawImgHeight || pImage->width != rawImgWidth)
		return;


	
	// Convert the pixel data from Flycapture to opencv
	int cvFmt = CV_16UC1;
	if (pixelFormat == 2)
		cvFmt = CV_16UC1;
	else
		cvFmt = CV_8UC1;
		
	unsigned int rowBytes = (double)pImage->getDataSizeInBytes()/(double)pImage->height;
	Mat opencvImg = Mat(pImage->height, 
						pImage->width, 
						cvFmt, 
						pImage->getDataPointer(),
						rowBytes );



	// Downsample image for processing
	resize(opencvImg,thImg,Size(),1.0/downsampleFactor,1.0/downsampleFactor,INTER_LINEAR);
	
	// Render the settings window to house the checkbox
	// and the trackbars below.
	//cvui::window(thImg, 10, 50, 180, 180, "Edge Settings");

	if (toLower(procMethod) == "laplacian") {

		// Filter the image with laplacian
		if (pixelFormat == 2) {
			Laplacian(thImg,thImg,CV_16UC1,laplaceSize);
			// Convert the 8-bit scale
			thImg.convertTo(thImg,CV_8UC1,0.015625/4);// 16-bit to 8-bit scaling
		
		}
		else {
			Laplacian(thImg,thImg,CV_8UC1,laplaceSize);
		}
		// Threshold
		threshold( thImg, thImg1, bwThreshold, 255,THRESH_BINARY);
		dilate(thImg1,bw1,sel7,Point(-1,-1));
		erode(bw1,bw2,sel7,Point(-1,-1));
	}
	else if (toLower(procMethod) == "canny") {
		// Run canny detector
		// Filter the image with laplacian
		if (pixelFormat == 2) {
			// Convert the 8-bit scale
			thImg.convertTo(thImg,CV_8UC1,0.015625/4);// 16-bit to 8-bit scaling
		}
		Canny(thImg,thImg1,cannyLow,cannyHigh,cannyKernelSize);
		dilate(thImg1,bw1,sel7,Point(-1,-1));
		erode(bw1,bw2,sel7,Point(-1,-1));
	}
	else {
		// Run canny detector
		// Convert the 8-bit scale
		// Filter the image with laplacian
		if (pixelFormat == 2) {
			// Convert the 8-bit scale
			thImg.convertTo(thImg,CV_8UC1,0.015625/4);// 16-bit to 8-bit scaling
		}
		Canny(thImg,thImg1,cannyLow,cannyHigh,cannyKernelSize);
		dilate(thImg1,bw1,sel7,Point(-1,-1));
		erode(bw1,bw2,sel7,Point(-1,-1));
	}
	
	// Render the settings window to house the checkbox
	// and the trackbars below.

	// Find contours in the binary image
	findContours( bw2, contours, hierarchy, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	// Limit the number of ROIs to process
	int roiCount = 0;
	int allRoiCount = 0;
	double averageRoiArea = 0.0;
	double averageRoiMal = 0.0;
	double maxRoiArea = -1.0;
	double maxRoiMal = -1.0;

	// Examine and save valid contours as individual ROIs
	for (unsigned int i = 0;i < contours.size();i++) {

		Rect bb = boundingRect(contours[i]);
		
		// Important! Compute area in raw pixels, not downsampled pixels
		double area = contourArea(contours[i])*downsampleFactor*downsampleFactor;

		// Fit an ellipse to get major axis length
		double mal = 0.0;
		if (contours[i].size() >= 6) {
			RotatedRect minEllipse = fitEllipse(contours[i]);
			if (minEllipse.size.width > minEllipse.size.height)
				mal = minEllipse.size.width*downsampleFactor;
			else
				mal = minEllipse.size.height*downsampleFactor;
		}

		bool saveRoi = false;
		
		// Check the ROI for size and length filters
		if (checkArea && (area > minArea) && (area < maxArea))
			saveRoi = true;
		if (checkMal && (mal > minMal) && (mal < maxMal))
			saveRoi = true;

		// Count rois with more that 36 pixels total
		if (area > minContourArea)
			allRoiCount++;

		// Save ROI if valid
		if (saveRoi) {

			roiCount++; // counter to limit max rois per frame
			totalRois++; // total roi counter
			recentRoiCount++; // counter resest each time it is queried
			
			// Average the object area and mal
			averageRoiArea += area;
			averageRoiMal += mal;
			
			// Get the max object area and mal
			if (area > maxRoiArea)
				maxRoiArea = area;
			if (mal > maxRoiMal)
				maxRoiMal = mal;

			// Save the roi with padding around it
			roiX = downsampleFactor*(bb.x-bb.width/2);
			roiY = downsampleFactor*(bb.y-bb.height/2);
			roiW = downsampleFactor*(bb.width+bb.width);
			roiH = downsampleFactor*(bb.height+bb.height);

			// Range check the rect
			//bool useOldROI = false;
			if (roiX < 0)
				roiX = 0;
			if (roiY < 0)
				roiY = 0;
			if (roiX+roiW >= opencvImg.cols)
				roiW = opencvImg.cols - roiX - 1;
			if (roiY+roiH >= opencvImg.rows)
				roiH = opencvImg.rows - roiY - 1;

			// Increment the number of objects found

			// Create the file name for the ROI including time, and bounding box info
			sprintf( filename, "%s-%llu-%llu-%06d-%03d-%d-%d-%d-%d.tif",
				cameraName.c_str(), pImage->systemMicros, pImage->cameraMicros, pImage->frameNumber, i, roiX,roiY,roiW,roiH);
			Poco::Path outputFile(dataDir,filename);

			// Extract the BB from the raw image
			Rect orgBB(roiX,roiY,roiW,roiH);
			Mat roi(opencvImg,orgBB);
			
			// Write the original pixels in the BB from the raw image to the file
			imwrite(outputFile.toString().c_str(),roi);

		}

		// Break if we have processed maxRoisPerImage
		if (roiCount >= maxRoisPerImage)
			break;
	}
    
    if (roiCount != 0) {
        averageRoiArea /= roiCount;
		averageRoiMal /= roiCount;
	}

	if (saveShrink) {
	
		// Code to save downsampled and segmented image
		sprintf( filename, "SHRINK-%d-%s-%llu-%llu-%06d.tif",
			shrinkThreshold, cameraName.c_str(), pImage->systemMicros, pImage->cameraMicros, pImage->frameNumber);
		Poco::Path outputFile(shrinkDir,filename);
		// Write the original pixels in the BB from the raw image to the file
        // Downsample image for processing
        
        resize(thImg,subSampleImg,Size(),1.0/shrinkSubSample,1.0/shrinkSubSample,INTER_LINEAR);
		threshold( subSampleImg, subSampleThresh, shrinkThreshold, 255,THRESH_TOZERO);
		imwrite(outputFile.toString().c_str(),subSampleThresh);
	}
    
    if (saveNetworkImage) {
    
    	// Code to save downsampled and segmented image
		sprintf( filename, "netimg%03d.jpg",networkImageIndex++);
        if (networkImageIndex >= numNetworkImages)
            networkImageIndex = 0;
		Poco::Path outputFile(networkDir,filename);
		
		// Draw bounding boxes on image
		for (unsigned int i = 0;i < contours.size();i++) {

			Rect bb = boundingRect(contours[i]);
			
			// Important! Compute area in raw pixels, not downsampled pixels
			double area = contourArea(contours[i])*downsampleFactor*downsampleFactor;

			// Fit an ellipse to get major axis length
			double mal = 0.0;
			if (contours[i].size() >= 6) {
				RotatedRect minEllipse = fitEllipse(contours[i]);
				if (minEllipse.size.width > minEllipse.size.height)
					mal = minEllipse.size.width*downsampleFactor;
				else
					mal = minEllipse.size.height*downsampleFactor;
			}

			bool saveRoi = false;
			
			// Check the ROI for size and length filters
			if (checkArea && (area > minArea) && (area < maxArea))
				saveRoi = true;
			if (checkMal && (mal > minMal) && (mal < maxMal))
				saveRoi = true;

			// Save ROI if valid
			if (saveRoi) {
				rectangle( thImg, bb.tl(), bb.br(), Scalar(255), 2, 8, 0 );
			}
		}
		
		// Write the original pixels in the BB from the raw image to the file
        // Downsample image for processing
        resize(thImg,subSampleImg,Size(),1.0/networkSubSample,1.0/networkSubSample,INTER_LINEAR);
		imwrite(outputFile.toString().c_str(),subSampleImg);
    }
    
	// Make sure to delete the image after processing.
	delete pImage;
	
	// Add stats to queues for retrival by other processes 
	lock();
	areaHistory.push(averageRoiArea);
	countHistory.push(roiCount);
	allCountHistory.push(allRoiCount);
	malHistory.push(averageRoiMal);
	maxAreaHistory.push(maxRoiArea);
	maxMalHistory.push(maxRoiMal);
	unlock();
	busy = false;
	
	
}

bool RoiImageProcessor::isBusy() {
	return busy;
}

bool RoiImageProcessor::isFull() {
	return dataQueue.size() >= maxQueueSize;
}

void RoiImageProcessor::setSaveToDisk(bool save) {
	saveToDisk = save;
}

int RoiImageProcessor::getTotalRois() {
	return totalRois;
}

int RoiImageProcessor::getRecentRoiCount() {
	int tmp = recentRoiCount;
	lock();
	recentRoiCount = 0;
	unlock();
	return tmp;
}

string RoiImageProcessor::status() {
	
	stringstream output;
	double avgArea = 0.0;
	double avgMal = 0.0;
	double avgCount = 0.0;
	double avgAllCount = 0.0;
	double maxArea = 0.0;
	double maxMal = 0.0;
	unsigned int samples = areaHistory.size();
	if (samples > 0) {
		// Average the roi area and counts
		lock();
		for (unsigned int i=0;i<samples;i++) {
			avgArea += areaHistory.front();
			areaHistory.pop();
			avgCount += countHistory.front();
			countHistory.pop();
			avgAllCount += allCountHistory.front();
			allCountHistory.pop();
			avgMal += malHistory.front();
			malHistory.pop();
			if (maxAreaHistory.front() > maxArea)
				maxArea = maxAreaHistory.front();
			maxAreaHistory.pop();
			if (maxMalHistory.front() > maxMal)
				maxMal = maxMalHistory.front();
			maxMalHistory.pop();
		}
		unlock();
		avgArea /= samples;
		avgCount /= samples;	
		avgAllCount /= samples;
		avgMal /= samples;
	}
	output << avgAllCount << "," << avgCount << "," << avgArea << "," << maxArea << "," << avgMal << "," << maxMal << "," << dataQueue.size();
	return output.str();
}

