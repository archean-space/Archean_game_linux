// DO NOT EDIT THIS FILE
/* 
	MODDING GUIDE (C/C++)

	1. Get yourself an official VENDOR name on the official website
	
	2. Come up with a name for your mod that is all lowercase with a maximum of 12 alphanumeric characters, starting with a letter
	
	For the following, replace words in square brackets [] with your information without the brackets

	4. Create a directory [path_to_game]/modules/[MYVENDOR]_[mymod]/

	5. Create a file [mymod].c in that directory that will contain the source code for your mod
	
		Example source code in [mymod].c:
			
			#include "../ARCHEAN_Planeterrain.h" // add this exact line once at the very beginning of your source code
			
			ARCHEAN_FUNCTION( Load ) {
				// this code will be executed when your mod is loaded
			}
			
			ARCHEAN_FUNCTION( Unload ) {
				// this code will be executed when your mod is unloaded
			}
			
			//... more functions defined in documentation

	6. Compile your mod with the following command: gcc -shared -o [MYVENDOR]_[mymod].ARCHEAN_Planeterrain.[dll|so] [mymod].c
	
	7. Set [MYVENDOR]_[mymod] as the value for the 'module' field in your planet config file 'terrain' section
	
	8. Enjoy your mod!
*/

#ifndef _ARCHEAN_PLANETERRAIN_H
#define _ARCHEAN_PLANETERRAIN_H

#define ARCHEAN_PLANETERRAIN_VERSION 0

#ifdef XENON_GAME // Only defined within the game's source code. Do NOT define XENON_GAME in your mod.
	#include "ARCHEAN_Planeterrain.hh"
#else
	#define ARCHEAN_PLANETERRAIN_DEFINE(...) 
	#ifdef __cplusplus
		extern "C" int _ARCHEAN_Planeterrain_VERSION() {return ARCHEAN_PLANETERRAIN_VERSION;}
		#define ARCHEAN_FUNCTION(F) extern "C" void F (struct F ## _PARAMS * Params_)
	#else
		int _ARCHEAN_Planeterrain_VERSION() {return ARCHEAN_PLANETERRAIN_VERSION;}
		#define ARCHEAN_FUNCTION(F) void F (struct F ## _PARAMS * Params_)
	#endif
	#define PARAM (*Params_)
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef XENON_GAME
	#define GLM_FORCE_INTRINSICS
	#define GLM_FORCE_SIMD_AVX2
	#define GLM_FORCE_CXX17
	#define GLM_FORCE_RADIANS
	#define GLM_FORCE_DEPTH_ZERO_TO_ONE
	#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <functional>
#include <sstream>

using namespace glm;

template<typename TK, typename TV, size_t N>
class fixed_map {
	struct element {
		TK key;
		TV value;
	};
	size_t nb_elements;
	element elements[N];
public:
	class iterator {
		const element* ptr;
	public:
		iterator(const element* ptr_) : ptr(ptr_) {}
		iterator operator++() {++ptr; return *this;}
		bool operator!=(const iterator& other) const {return ptr != other.ptr;}
		const element& operator*() const {return *ptr;}
	};
	template<typename TKK>
	fixed_map(const std::unordered_map<TKK, TV>& in_map = {}) : nb_elements(0) {
		for (const auto&[key,value] : in_map) {
			assert(nb_elements < N);
			if constexpr (std::is_same_v<TKK, TK>) {
				elements[nb_elements++] = element{.key = key, .value = value};
			} else {
				static_assert(std::is_same_v<TKK, std::string>);
				if constexpr (std::is_same_v<TK, const char*>) {
					elements[nb_elements++] = element{.key = key.c_str(), .value = value};
				} else {
					TK k;
					std::stringstream s {key};
					s >> k;
					elements[nb_elements++] = element{.key = k, .value = value};
				}
			}
		}
	}
	iterator begin() const {return iterator{elements};}
	const iterator end() const {return iterator{elements + nb_elements};}
	TV operator[] (TK key) const {
		for (const auto&[k,v] : *this) {
			if constexpr (std::is_same_v<TK, const char*>) {
				if (strcmp(key, k) == 0) return v;
			} else {
				if (key == k) return v;
			}
		}
		return {};
	}
};

struct MakeTerrain_PARAMS {
	fixed_map<const char*, double, 256> in_configs;
	uint64_t out_index;
	void* out_configs_ptr;
	size_t out_configs_size;
	const char* out_computeShader;
	const char* out_clutterComputeShader;
	uint32_t out_clutterDensityPerChunk; // 0-32
	// const char* out_surfaceShader; // NOT WORKING YET BECAUSE IT WOULD ADD THE SURFACE TYPE AFTER THE RENDERER IS ALREADY INITIALIZED
};

struct GetTerrainHeightMap_PARAMS {
	dvec3 in_normalizedPos;
	uint64_t in_index;
	double out_height;
};


ARCHEAN_PLANETERRAIN_DEFINE(
	MakeTerrain
	,GetTerrainHeightMap
)


#endif
