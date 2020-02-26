#include "slicer.h"

VedioSlicer::VedioSlicer(String filePath)
{
	vedio.open(filePath);
	frameNum = (int)vedio.get(CAP_PROP_FRAME_COUNT);
}

Mat VedioSlicer::GetNextFrame()
{
	Mat img;
	return vedio.read(img) ? img : Mat();
}

int VedioSlicer::GetTotalFrames()
{
	return vedio.get(CAP_PROP_FRAME_COUNT);
}

bool VedioSlicer::SetCurrentFrame(int frameIndex)
{
	return vedio.set(CAP_PROP_POS_FRAMES, frameIndex);
}

int VedioSlicer::GetCurrentFrame()
{
	return vedio.get(CAP_PROP_POS_FRAMES);
}
