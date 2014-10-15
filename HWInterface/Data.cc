/*

    FileName :                     Data.cc
    Content :                      Data handling from DAQ
    Programmer :                   Nicolas PIERRE
    Version :                      1.0
    Date of creation :             10/07/14
    Support :                      mail to : nicolas.pierre@icloud.com

 */

#include "Data.h"
#include <iostream>

namespace Ph2_HwInterface
{
	//Data Class

	Data::Data( BeBoard& pBoard, uint32_t pNbCbc ) :
		fBuf( 0 ),
		fCurrentEvent( 0 ),
		fEvent( NULL ),
		fEventSize( 0 )
	{
	}

	Data::Data( Data& pD )
	{
		fBuf = 0;
		// Initialise( pD.fNevents );
		fBufSize = pD.fBufSize;
		fNevents = pD.fNevents;
		fCurrentEvent = pD.fCurrentEvent;
		fNCbc = pD.fNCbc;
		fEvent = pD.fEvent;
		fEventSize = pD.fEventSize;
	}


	void Data::Set( std::vector<uint32_t>* pData, uint32_t pNevents )
	{

		// initialize the buffer data array and the buffer size (one 32 bit word is 4 char!)
		fBufSize = pData->size() * 4;
		fNevents = uint32_t( pNevents );
		fEventSize = uint32_t( fBufSize / fNevents );
		fNCbc = ( fEventSize - ( EVENT_HEADER_TDC_SIZE_CHAR ) ) / ( CBC_EVENT_SIZE_CHAR );

		std::cout << "Initializing buffer with " << pData->size() << " 32 bit words and " << fBufSize << " chars containing data from " << fNevents << "  Events with an eventbuffer size of " << fEventSize << " and " << fNCbc << " CBCs each! " << EVENT_HEADER_TDC_SIZE_CHAR << " " << CBC_EVENT_SIZE_CHAR << std::endl;
		if ( fBuf ) free( fBuf );
		fBuf = ( char* )malloc( pData->size() * 4 );

		Reset();

		for ( unsigned int i = 0; i < pData->size(); i++ )
		{

			char  cSwapped[4];
			uint32_t cVal = pData->at( i );
			// char buf[4];
			// buf[0] = pData->at( i ) >> 24;
			// buf[1] = pData->at( i ) >> 16;
			// buf[2] = pData->at( i ) >> 8;
			// buf[3] = pData->at( i );
			swapByteOrder( ( const char* )  &cVal , cSwapped, 4 );

			for ( int j = 0; j < 4; j++ )
				fBuf[i * 4 + j] = cSwapped[j];
		}
	}


	// void Data::Initialise( uint32_t pNevents )
	// {
	//  fNevents = uint32_t( pNevents );
	//  // fBufSize = ( fNevents + 1 ) * fEvent.fEventSize ;  //is already in char
	//  // if ( fBuf )
	//  //  free( fBuf );
	//  // fBuf = ( char* ) malloc( fBufSize );
	//  // std::cout << "Initializing buffer for " << fNevents << " Events with a size of  " << fBufSize << " chars" << std::endl;
	//  // fEvent.Clear();

	//  // #ifdef __CBCDAQ_DEV__
	//  //      std::cout << "Data::Initialise done." << std::endl;
	//  // #endif

	// }


	// void Data::Initialise( uint32_t pNevents, BeBoard& pBoard )
	// {

	//  fNevents = uint32_t( pNevents );
	//  // fBufSize = ( fNevents ) * fEvent.fEventSize ;
	//  // if ( fBuf )
	//  //  free( fBuf );
	//  // fBuf = ( char* ) malloc( fBufSize );
	//  // std::cout << "Initializing buffer for " << fNevents << " Events with a size of  " << fBufSize << " chars" << std::endl;

	//  // fEvent.Clear();

	//  // fEvent.AddBoard( pBoard );

	//  // #ifdef __CBCDAQ_DEV__
	//  //      std::cout << "Data::Initialise done." << std::endl;
	//  // #endif

	// }


	void Data::Reset()
	{
		fCurrentEvent = 0;

		for ( uint32_t i = 0; i < fBufSize; i++ )
			fBuf[i] = 0;
	}


	void Data::CopyBuffer( Data& pData )
	{
		memcpy( fBuf, pData.fBuf, pData.fBufSize );
	}


	// void Data::swapByteOrder( const char* org, char* swapped, unsigned int nbyte )
	// {
	//  for ( unsigned int i = 0; i < nbyte; i++ )
	//      swapped[i] = org[nbyte - 1 - i];
	// }

	void Data::swapByteOrder( const char* org, char* swapped, unsigned int nbyte )
	{
		swapped[0] = org[3];
		swapped[1] = org[2];
		swapped[2] = org[1];
		swapped[3] = org[0];
	}


	const char* Data::GetBuffer( uint32_t& pBufSize ) const
	{
		pBufSize = fBufSize;
		return fBuf;
	}


	const Event* Data::GetNextEvent( BeBoard* pBoard )
	{
		if ( fCurrentEvent >= fNevents ) return 0;
		else
		{
			if ( fEvent ) delete fEvent;
			fEvent = new Event( pBoard, fNCbc, &fBuf[ fCurrentEvent * fEventSize ] );
			std::cout << "Get Next Event Event " << fCurrentEvent << " and position in buffer " << fCurrentEvent* fEvent->fEventSize << std::endl;
			fCurrentEvent++;
			return fEvent;
		}
	}

}
