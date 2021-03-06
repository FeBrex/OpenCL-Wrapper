//Copyright (C) 2013 Cem Bassoy.
//
//This file is part of the OpenCL Utility Toolkit.
//
//OpenCL Utility Toolkit is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//OpenCL Utility Toolkit is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with OpenCL Utility Toolkit.  If not, see <http://www.gnu.org/licenses/>.

#include <algorithm>
#include <cstring>
#include <memory>
#include <stdexcept>

#include <ocl_device.h>
#include <ocl_query.h>
#include <ocl_queue.h>
#include <ocl_platform.h>

#include <utl_assert.h>


/*! \brief Instantiates this Device.
  *
  * Usually the devices are created using the Platform class.
  * \param dev is a OpenCL device which is identified with cl_device_id.
  */
ocl::Device::Device(cl_device_id dev) :
    _id(dev), _type(ocl::device_type::ALL)
{
	cl_device_type t;
	OPENCL_SAFE_CALL( clGetDeviceInfo (_id,CL_DEVICE_TYPE, sizeof(t), &t, NULL) );
    _type = ocl::DeviceType::type(t);

}

/*! \brief Instantiates this Device.
  *
  * No OpenCL device specified. Must do this later.
  */
ocl::Device::Device() :
    _id(0), _type(ocl::device_type::ALL)
{

}

bool ocl::Device::supportsVersion( int major, int minor ) const
{
  int mjr = 0, mnr = 0;
  
  std::sscanf( version().c_str(), "OpenCL %i.%i", &mjr, &mnr );
  
  return mjr > major || (mjr == major && mnr >= minor);
}

#ifdef OPENCL_V1_2
static bool supportsAtLeast1Point2( cl_platform_id id )
{
  char version[128];
  size_t versionLen = 0;
  
  clGetPlatformInfo( id, CL_PLATFORM_VERSION, 128, version, &versionLen );
  
  int major = 0, minor = 0;
  
  std::sscanf( version, "OpenCL %i.%i", &major, &minor );
  
  return major > 1 || (major == 1 && minor > 1);
}
#endif

/*! \brief Destructs this Device.
  *
  */
ocl::Device::~Device()
{
  if ( _id )
  {
#ifdef OPENCL_V1_2
    if ( supportsAtLeast1Point2( platform() ) )
      OPENCL_SAFE_CALL( clReleaseDevice( _id ) );
#endif
    _id = 0;
  }
}


/*! \brief Instantiates this Device using anther device.
  *
  * Note that both Device instance dev and this refer to
  * the same OpenCL device cl_device_id.
  * \param dev Device from which this Device is created.
  */
ocl::Device::Device(const Device& dev) :
    _id(dev._id), _type(dev._type)
{
#ifdef OPENCL_V1_2
  if ( supportsAtLeast1Point2( platform() ) )
    OPENCL_SAFE_CALL( clRetainDevice( _id ) );
#endif
}

/*! \brief Copies from other device to this device
  *
  * Note that both Device instance dev and this refer to
  * the same OpenCL device cl_device_id.
  * \param dev Device from which this Device is created.
  */
ocl::Device& ocl::Device::operator =(const ocl::Device &dev)
{
  if ( this != &dev )
  {
    _id = dev._id;
    _type = dev._type;
    
#ifdef OPENCL_V1_2
    if ( supportsAtLeast1Point2( platform() ) )
      OPENCL_SAFE_CALL( clRetainDevice( _id ) );
#endif
  }
  
  return *this;
}


/*! \brief Sets the id of this Device
  *
  * Note that you have to change the type if
  * the cl_device_id does not refer to the same type.
  * \param id new cl_device_id to be set.
  */
void ocl::Device::setId(cl_device_id id)
{
    this->_id = id;
}

/*! \brief Returns the id of this Device
  *
  */
cl_device_id ocl::Device::id() const
{
	return this->_id;
}

/*! \brief Returns the type of this Device
  *
  */
const ocl::DeviceType& ocl::Device::type() const
{
	return this->_type;
}


/*! \brief Returns true if the ids are the same.
  *
  */
bool ocl::Device::operator == (const Device & dev) const
{
	return dev.id() == this->id();
}

/*! \brief Returns false if the ids are the same.
  *
  */
bool ocl::Device::operator != (const Device & dev) const
{
	return dev.id() != this->id();
}

/*! \brief Returns true if the ids are the same.
  *
  */
bool ocl::Device::operator == (cl_device_id  dev) const
{
	return dev == this->id();
}

/*! \brief Returns false if the ids are the same.
  *
  */
bool ocl::Device::operator != (cl_device_id  dev) const
{
	return dev != this->id();
}

/*! \brief Returns true if the types are the same.
  *
  */
bool ocl::Device::operator == (const ocl::DeviceType&  type) const
{
	return type == this->type();
}

/*! \brief Returns false if the types are the same.
  *
  */
bool ocl::Device::operator != (const ocl::DeviceType&  type) const
{
	return type != this->type();
}

/*! \brief Returns the true if this Device is a GPU . */
bool ocl::Device::isGpu() const
{
    return this->type() == CL_DEVICE_TYPE_GPU;
}

/*! \brief Returns the true if this Device is a CPU. */
bool ocl::Device::isCpu() const
{
	return this->type() == CL_DEVICE_TYPE_CPU;
}

/*! \brief Returns the true if this Device is an accelerator (Cell). */
bool ocl::Device::isAccelerator() const
{
	return this->type() == CL_DEVICE_TYPE_ACCELERATOR;
}


/*! \brief Returns the maximum compute units for a given device. */
size_t ocl::Device::maxComputeUnits() const
{
	cl_uint maxCompUnits;
	OPENCL_SAFE_CALL(  clGetDeviceInfo (_id, CL_DEVICE_MAX_COMPUTE_UNITS , sizeof(maxCompUnits), &maxCompUnits, NULL) );
	return size_t(maxCompUnits);
}

/*! \brief Returns the maximum dimensions that specify the global and local work-item IDs for *this. */
size_t ocl::Device::maxWorkItemDim() const
{
	cl_uint maxWorkItemDim;
	OPENCL_SAFE_CALL( clGetDeviceInfo (_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS , sizeof(maxWorkItemDim), &maxWorkItemDim, NULL) );
	return size_t(maxWorkItemDim);
}

/*! \brief Returns the maximum number of work-items that can be specified in each dimension of the work-group for *this. */
std::vector<size_t> ocl::Device::maxWorkItemSizes() const
{
    std::vector<size_t> a(3,0);
    //size_t a[3];
    OPENCL_SAFE_CALL( clGetDeviceInfo (this->id(), CL_DEVICE_MAX_WORK_ITEM_SIZES , sizeof(size_t)*3, a.data(), NULL) );
    //std::vector<size_t> b(a, a + sizeof(a) / sizeof(int));
    return a;
}

/*! \brief Returns the maximum number of work-items in a work- group executing a kernel on a single compute unit for *this. */
size_t ocl::Device::maxWorkGroupSize() const
{
    size_t a;
	OPENCL_SAFE_CALL( clGetDeviceInfo (this->id(), CL_DEVICE_MAX_WORK_GROUP_SIZE , sizeof(a), &a, NULL) );
	return a;
}

/*! \brief Returns the maximum size in bytes of a constant buffer allocation for *this. */
size_t ocl::Device::maxConstantBufferSize() const
{
    cl_ulong a;
	OPENCL_SAFE_CALL( clGetDeviceInfo (this->id(), CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE , sizeof(a), &a, NULL) );
	return size_t(a);
}


/*! \brief Returns the maximum size of memory object allocation in bytes for *this . */
size_t ocl::Device::maxMemAllocSize() const
{
    cl_ulong maxMemAllocSize;
    OPENCL_SAFE_CALL(  clGetDeviceInfo (_id, CL_DEVICE_MAX_MEM_ALLOC_SIZE , sizeof(maxMemAllocSize), &maxMemAllocSize, NULL) );
    return size_t(maxMemAllocSize);
}

/*! \brief Returns the global memory size in bytes for *this . */
size_t ocl::Device::globalMemSize() const
{
    cl_ulong a;
    OPENCL_SAFE_CALL(  clGetDeviceInfo (_id, CL_DEVICE_GLOBAL_MEM_SIZE , sizeof(a), &a, NULL) );
    return size_t(a);
}

/*! \brief Returns the local memory size in bytes for *this . */
size_t ocl::Device::localMemSize() const
{
    cl_ulong a;
    OPENCL_SAFE_CALL(  clGetDeviceInfo (_id, CL_DEVICE_LOCAL_MEM_SIZE , sizeof(a), &a, NULL) );
    return size_t(a);
}

/*! \brief Returns the OpenCL platform on which this Device is located.*/
cl_platform_id ocl::Device::platform() const
{
	cl_platform_id _pl;
	OPENCL_SAFE_CALL( clGetDeviceInfo (this->id(), CL_DEVICE_PLATFORM , sizeof(_pl), &_pl, NULL) );
	TRUE_ASSERT(_pl != 0, "Platform not found");
	return _pl;
}

static std::string 
getDeviceInfo(cl_device_id id, cl_device_info info)
{
	size_t size = 0u;
	OPENCL_SAFE_CALL( clGetDeviceInfo(id, info, 0u, nullptr, &size ) );
	std::unique_ptr< char[] > buffer( new char[size] );
 
	OPENCL_SAFE_CALL( clGetDeviceInfo(id, info,  size, buffer.get(), NULL));
	return buffer.get();
}

/*! \brief Returns the version of this Device .*/
std::string ocl::Device::version() const
{
	return getDeviceInfo(this->id(),CL_DEVICE_VERSION);
}

/*! \brief Returns the name of this Device .*/
std::string ocl::Device::name() const
{
	return getDeviceInfo(this->id(),CL_DEVICE_NAME);
}

/*! \brief Returns the name of the vendor of this Device .*/
std::string ocl::Device::vendor() const
{
	return getDeviceInfo(this->id(),CL_DEVICE_VENDOR);
}

/*! \brief Returns all extensions of this Device (support of double precision?) .*/
std::string ocl::Device::extensions() const
{
	return getDeviceInfo(this->id(),CL_DEVICE_EXTENSIONS);
}

/*! \brief Prints this Device.*/
void ocl::Device::print() const
{
    std::cout << "\tDevice " << std::endl;
    std::cout << "\t\tVendor: " << this->vendor() << std::endl;
    std::cout << "\t\tName: " <<  this->name() << std::endl;
}

static bool supportsExtension( std::string const& extensionsString, char const* extension )
{
 auto const len = std::strlen( extension );
 auto p = extensionsString.c_str();

 while ( *p != '\0' )
 {
		auto n = std::strcspn( p, " " );

		if ( len == n && 0 == strncmp( p, extension, n ) ) return true;
		p += ++n;
	}

 return false;
}


/*! \brief Return true if this Device supports images.*/
bool ocl::Device::imageSupport() const
{
	cl_bool support = CL_FALSE;
	OPENCL_SAFE_CALL( clGetDeviceInfo( this->id(), CL_DEVICE_IMAGE_SUPPORT, sizeof support, &support, NULL ) );
	return support == CL_TRUE;
}

bool ocl::Device::supportsExtension( std::string const& ext ) const
{
  return ::supportsExtension( extensions(), ext.c_str() );
}


/** 
 * @retval true If this device supports double data type (cl_khr_fp64).
 * @retval false Otherwise.
 */
bool ocl::Device::doubleSupport() const
{
  return supportsExtension( "cl_khr_fp64" );
}


/**
 * Get the size in bytes of the L2 cache of the device.
 * This is done by name look up.
 */
size_t ocl::Device::getL2CacheSize() const
{
  std::string const n = name();
  
  if ( n == "Tahiti" )
  {
    return 768u * 1024u;
  }
  else if ( n == "Hawaii" )
  {
    // http://www.hardwareluxx.com/index.php/news/hardware/vgacards/28214-new-slide-reveals-hawaii-gpu-specs.html
    return 1024u * 1024u;
  }
  else if ( n == "Tesla C2050" )
  {
    return 786432u;
  }
  // see: https://software.intel.com/sites/default/files/article/393195/intel-xeon-phi-core-micro-architecture.pdf
  else if ( n == "Intel(R) Many Integrated Core Acceleration Card" )
  {
    // Each of the 60 cores has 512KB L2 cache. Thus the
    // L2 cache is not shared among the cores.
    return 512u * 1024u;
  }
  else if ( n == "        Intel(R) Core(TM) i5-2500 CPU @ 3.30GHz" || 
            n == "        Intel(R) Core(TM) i5-2300 CPU @ 2.80GHz")
  {
    // As for Intel Xeon Phi L2 cache is dedicated to each core.
    // L3 cache involves sharing.
    // see: http://www.intel.com/content/dam/www/public/us/en/documents/datasheets/2nd-gen-core-desktop-vol-1-datasheet.pdf
    return 256u * 1024u;
  }
  else
  {
    throw std::runtime_error( "don't know size of L2 cache for device " + n );
  }
}


size_t ocl::Device::wavefrontSize() const
{
  std::string const n = name();
  
  if ( n == "Tahiti" || n == "Hawaii" )
  {
    return 64u;
  }
  else if ( n == "Tesla C2050" )
  {
    return 32u;
  }
  // https://software.intel.com/sites/products/documentation/ioclsdk/2013/OG/Intel_R__SDK_for_OpenCL_Applications_2013_-_Optimization_Guide.pdf
  else if ( n == "        Intel(R) Core(TM) i5-2500 CPU @ 3.30GHz" )
  {
    return 64u;
  }
  // https://software.intel.com/en-us/articles/opencl-design-and-programming-guide-for-the-intel-xeon-phi-coprocessor
  // Intel calls this "vector size"
  else if ( n == "Intel(R) Many Integrated Core Acceleration Card" )
  {
    return 16u;
  }
  else if ( n == "AMD FX(tm)-8150 Eight-Core Processor" )
  {
    return 1;
  }
  else
  {
    throw std::runtime_error( "don't know wavefront size for device " + n );
  }
}


/**
 * Get the number of registers available per work-item such that the number of wavefronts per compute unit is not affected.
 */
size_t ocl::Device::maxWorkItemRegs() const
{
  std::string const n = name();
  
  // The guess is that Hawaii is not lesser equipped than Tahiti.
  if ( n == "Tahiti" || n == "Hawaii" )
  {
    return 25u;
  }
  else if ( n == "        Intel(R) Core(TM) i5-2500 CPU @ 3.30GHz" )
  {
    return 128; // Don't know, but should be many.
  }
  // Tesla C2050 is compute capability 2.0
  // see: https://developer.nvidia.com/cuda-gpus
  // Use the CUDA_Occupancy_calculator.xls
  else if ( n == "Tesla C2050" )
  {
    return 16;
  }
  // see: https://software.intel.com/sites/default/files/article/393195/intel-xeon-phi-core-micro-architecture.pdf
  else if ( n == "Intel(R) Many Integrated Core Acceleration Card" )
  {
    return 32u;
  }
  else if ( n == "AMD FX(tm)-8150 Eight-Core Processor" )
  {
    return 16; // just a guess
  }
  else
  {
    throw std::runtime_error( "don't know maximum number of registers per work item for device " + n );
  }
}


/**
 * Get the maximum number of bytes a work-group can allocate as local memory.
 */
size_t ocl::Device::maxLocalMemAllocSize() const
{
  std::string const n = name();
  
  if ( n == "Tahiti" || n == "Hawaii" )
  {
    return 32u * 1024u;
  }
  else if ( n == "        Intel(R) Core(TM) i5-2500 CPU @ 3.30GHz" )
  {
    return 128u * 1024u; // This is for Intel HD Graphics
  }
  // Tesla has configurable shared memory, which can be either 16KB or 48KB.
  // 48KB is the default value and the remaining KB are used for L1 caching.
  else if ( n == "Tesla C2050" )
  {
    return localMemSize(); //48u * 1024u;
  }
  else if ( n == "Intel(R) Many Integrated Core Acceleration Card" )
  {
    // Intel Xeon Phi does not really have limitations on local memory :(
    return 8192u * 1024u; // Tested with example Kernel. 
  }
  else
  {
    throw std::runtime_error( "don't know maximum size to allocate for local memory for device " + n );
  }
}


/**
 * Get maximum size of local memory that a work-group can allocate without affecting the number of active wavefronts per compute unit.
 */
size_t ocl::Device::localMemSizeNotLimitedByWavefronts( size_t const workGroupSize ) const
{
  if ( workGroupSize > maxWorkGroupSize() ) throw std::runtime_error("too large work-group size");
  
  std::string const n = name();
  
  // Nobody nows about Hawaii, but Tahiti should be a good approximartion here.
  if ( n == "Tahiti" || n == "Hawaii" )
  {
    return localMemSize() / ((workGroupSize + wavefrontSize() - 1u) / wavefrontSize());
  }
  else if ( n == "Tesla C2050" )
  {
    if ( workGroupSize <= 192 ) return 6144;
    if ( workGroupSize <= 256 ) return 8192;
    if ( workGroupSize <= 384 ) return 12288;
    if ( workGroupSize <= 512 ) return 16384;
    if ( workGroupSize <= 768 ) return 24576;
    return 49152;
  }
  else if ( n == "Intel(R) Many Integrated Core Acceleration Card" )
  {
    // Intel Xeon Phi does not really have limitations on local memory :(
    return maxLocalMemAllocSize();
  }
  else if ( n == "AMD FX(tm)-8150 Eight-Core Processor" )
  {
    return maxLocalMemAllocSize();
  }
  else
  {
    throw std::runtime_error( "don't know local memory limit on active wavefronts for device " + n );
  }
}
