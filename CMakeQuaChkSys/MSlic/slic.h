/**********************************************************************************
 *                  copyright	:	whudepgrid
 *					author		:	tankai
 *					create time	:	2017.02.22
 *					function	:	from buffer data to superpixels laber 
***********************************************************************************/

#if !defined(_SLIC_H_INCLUDED_)
#define _SLIC_H_INCLUDED_

#ifdefine DTKSLICCLASSINFOR
#define TKSLICCLASSDLL _declspec(dllexport)
#else
#define TKSLICCLASSDLL _declspec(dllimport)

#include <vector>
#include <string>
#include <algorithm>
using namespace std;
typedef unsigned short UINT;
typedef unsigned char uchar;

class TKSLICCLASSDLL SLIC  
{
public:
	SLIC();
	virtual ~SLIC();

	//@free the memory
	void Close();

	//===========================================================================
	///	tk:	Perform SLIC algorithm on the given image 
	/// with the given number of superpixels
	//===========================================================================
	void GenerateSuperpixels(
		uchar*	bufferGray,
		int		n_data_width,
		int		n_data_height,
		UINT	numSuperpixels);

	//===========================================================================
	///	Get label on each pixel which shows the number of superpixel it belongs to
	//===========================================================================
	uchar* GetLabel();

private:
	//============================================================================
	// tk:	Superpixel segmentation for a given number of superpixels
	//============================================================================
	void PerformSLICO_ForGivenK(
		const uchar*		ubuff,
		const int					width,
		const int					height,
		uchar*						klabels,
		int&						numlabels,
		const int&					K,//required number of superpixels
		const float&				m);//weight given to spatial distance
private:

	//============================================================================
	// Magic SLIC. No need to set M (compactness factor) and S (step size).
	// SLICO (SLIC Zero) varies only M dynamicaly, not S.
	//============================================================================
	void PerformSuperpixelSegmentation_VariableSandM(
		vector<float>&				kseedsl,
		vector<float>&				kseedsa,
		vector<float>&				kseedsb,
		vector<float>&				kseedsx,
		vector<float>&				kseedsy,
		uchar*						klabels,
		const int&					STEP,
		const int&					NUMITR);
	//============================================================================
	// Pick seeds for superpixels when step size of superpixels is given.
	//============================================================================
	void GetLABXYSeeds_ForGivenStepSize(
		vector<float>&				kseedsl,
		vector<float>&				kseedsa,
		vector<float>&				kseedsb,
		vector<float>&				kseedsx,
		vector<float>&				kseedsy,
		const int&					STEP,
		const bool&					perturbseeds,
		const vector<float>&		edgemag);
	//============================================================================
	// Pick seeds for superpixels when number of superpixels is input.
	//============================================================================
	void GetLABXYSeeds_ForGivenK(
		vector<float>&				kseedsl,
		vector<float>&				kseedsa,
		vector<float>&				kseedsb,
		vector<float>&				kseedsx,
		vector<float>&				kseedsy,
		const int&					STEP,
		const bool&					perturbseeds,
		const vector<float>&		edges);

	//============================================================================
	// Move the seeds to low gradient positions to avoid putting seeds at region boundaries.
	//============================================================================
	void PerturbSeeds(
		vector<float>&				kseedsl,
		vector<float>&				kseedsa,
		vector<float>&				kseedsb,
		vector<float>&				kseedsx,
		vector<float>&				kseedsy,
		const vector<float>&		edges);
	//============================================================================
	// Detect color edges, to help PerturbSeeds()
	//============================================================================
	void DetectLabEdges(
		const float*				lvec,
		const float*				avec,
		const float*				bvec,
		const int&					width,
		const int&					height,
		vector<float>&				edges);
	//============================================================================
	// xRGB to XYZ conversion; helper for RGB2LAB()
	//============================================================================
	void RGB2XYZ(
		const int&					sR,
		const int&					sG,
		const int&					sB,
		float&						X,
		float&						Y,
		float&						Z);
	//============================================================================
	void RGB162XYZ(
		const int&					sR,
		const int&					sG,
		const int&					sB,
		float&						X,
		float&						Y,
		float&						Z);
	//============================================================================
	// sRGB to CIELAB conversion
	//============================================================================
	void RGB2LAB(
		const int&					sR,
		const int&					sG,
		const int&					sB,
		float&						lval,
		float&						aval,
		float&						bval);
	//============================================================================
	void RGB162LAB(
		const int&					sR,
		const int&					sG,
		const int&					sB,
		float&						lval,
		float&						aval,
		float&						bval);

	//============================================================================
	// sRGB to CIELAB conversion for 2-D images
	//============================================================================
	void DoRGBtoLABConversion(
		const UINT*&		ubuff,
		float*&					lvec,
		float*&					avec,
		float*&					bvec);

	//============================================================================
	void DoRGB16toLABConversion(
		const unsigned long*&		ubuff,
		float*&					lvec,
		float*&					avec,
		float*&					bvec);
	//============================================================================
	// sRGB to CIELAB conversion for 3-D volumes
	//============================================================================
	void DoRGBtoLABConversion(
		const UINT**&		ubuff,
		float**&					lvec,
		float**&					avec,
		float**&					bvec);

	//============================================================================
	// Post-processing of SLIC segmentation, to avoid stray labels.
	//============================================================================
	void EnforceLabelConnectivity(
		const uchar*				labels,
		const int&					width,
		const int&					height,
		short*						nlabels,//input labels that need to be corrected to remove stray labels
		int&						numlabels,//the number of labels changes in the end if segments are removed
		const int&					K); //the number of superpixels desired by the user

private:
	int										m_width;
	int										m_height;
	int										m_depth;

	float*									m_lvec;
	float*									m_avec;
	float*									m_bvec;

	float**									m_lvecvec;
	float**									m_avecvec;
	float**									m_bvecvec;

	uchar*									bufferGray; // buffer if gray image
public:
	uchar*									label; // label record the superpixel pixel belongs to
};

#endif // !defined(_SLIC_H_INCLUDED_)
