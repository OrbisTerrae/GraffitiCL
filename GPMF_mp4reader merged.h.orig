/*! @file GPMF_mp4reader.h
 *
 *  @brief Way Too Crude MP4 reader 
 *
 *  @version 1.0.0
 * 
 *  (C) Copyright 2017 GoPro Inc (http://gopro.com/).
 *	
 *  Licensed under either:
 *  - Apache License, Version 2.0, http://www.apache.org/licenses/LICENSE-2.0  
 *  - MIT license, http://opensource.org/licenses/MIT
 *  at your option.
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#ifndef _GPMF_MP4READER_H
#define _GPMF_MP4READER_H

#include "GPMF_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

int verbose;
int nooutput;
int debug;

    
typedef struct SampleToChunk
{
	uint32_t chunk_num;
	uint32_t samples;
	uint32_t id;
} SampleToChunk;

#define MAX_SAMPLES 1080
typedef struct HMMT_struct
{
	uint32_t nb;
	uint32_t samples[MAX_SAMPLES];
} HMMT_struct;
typedef struct SETT_struct
{
	uint32_t set1;
	uint32_t set2;
	uint32_t set3;
} SETT_struct;

#define arraysize(ar) (sizeof(ar) / sizeof(ar[0]))
const char *
DecipherValue(const char** strarray, int max, int value);
double OpenGPMFSource(const char *filename);
double OpenGPMFSourceUDTA_HMMT(const char *filename, HMMT_struct* hmmt);
double OpenGPMFSourceUDTA_FIRM(const char *filename);
double OpenGPMFSourceUDTA_SETT(const char *filename, SETT_struct* sett);
double OpenGPMFSourceUDTA_VIDE(const char *filename);
void CloseGPMFSource(void);
uint32_t GetNumberGPMFPayloads(void);
uint32_t *GetGPMFPayload(uint32_t *lastpayload, uint32_t index);
void FreeGPMFPayload(uint32_t *lastpayload);
uint32_t GetGPMFPayloadSize(uint32_t index);
uint32_t GetGPMFPayloadTime(uint32_t index, double *in, double *out); //MP4 timestamps for the payload

#define GPMF_SAMPLE_RATE_FAST		0
#define GPMF_SAMPLE_RATE_PRECISE	1

double GetGPMFSampleRate(uint32_t fourcc, uint32_t flags);
double GetGPMFSampleRateAndTimes(GPMF_stream *gs, double lastrate, uint32_t index, double *in, double *out); //Jitter corrected sample(s) time, if lastrate is unknown, send 0.0 and it will be computed

double getGPMF_videolength();
uint32_t getMetasize_count();
double getMetadatalength();
int getMDLanguage();
int getMDQuality();
long getMDDuration();
long getMDTimeScale();
long getMDModificationTime();
long getMDCreationTime();
int getIndexcount();
long getClockdemon();
long getClockcount();
long getTrakClockdemon();
long getTrakClockcount();
long getMetaClockdemon();
long getMetaClockcount();


#ifdef __cplusplus
}
#endif

#endif
