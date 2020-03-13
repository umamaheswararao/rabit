/*!
 *  Copyright (c) 2014 by Contributors
 * \file engine_empty.cc
 * \brief this file provides a dummy implementation of engine that does nothing
 *  this file provides a way to fall back to single node program without causing too many dependencies
 *  This is usually NOT needed, use engine_mpi or engine for real distributed version
 * \author Tianqi Chen
 */
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#define NOMINMAX

#include "rabit/internal/engine.h"
#include "/home/xgboost/install/OneCCL/oneccl/build/_install/include/ccl.h"
#include <iostream>
#include <stdio.h>
#include <execinfo.h>
#include <stdlib.h>

ccl_status_t do_reduction_sum2(const void* in_buf, size_t in_count, void* inout_buf,
                              size_t* out_count, const ccl_fn_context_t* ctx, ccl_datatype_t dtype)
{
   // if(rabit::engine::ReduceHandle::redfunc_!= NULL) printf("redfunc_ is not null......\n");
    //else { printf("redfunc_ is null.......\n");}



    printf("do_reduction_sum2 called..........................................................UMA\n");   
    int idx=0;
    if (out_count) *out_count = in_count;
    switch (dtype)
    {
        case ccl_dtype_char:
            for (idx = 0; idx < in_count; idx++)
            {
                ((char*)inout_buf)[idx] += ((char*)in_buf)[idx];
            }
            break;
        case ccl_dtype_float:
            for (idx = 0; idx < in_count; idx++)
            {
                ((float*)inout_buf)[idx] += ((float*)in_buf)[idx];
            }
            break;
        default:
            break;
    }
    return ccl_status_success;
}
//using namespace ccl;
namespace rabit {
/**
namespace utils {
  bool STOP_PROCESS_ON_ERROR = true;
}
*/
namespace engine {
/*! \brief EmptyEngine */
class EmptyEngine : public IEngine {
 public: 
  EmptyEngine(void) {
    version_number = 0;
  }
  bool isInited = false; 
  
  
 // ccl_coll_attr_t coll_attr;
 // ccl_request_t request;
 // initialize the manager
 //bool Init(int argc, char* argv[]);
 //bool Shutdown(void);
 //void SetParam(const char *name, const char *val);
 //void TrackerPrint(const std::string &msg);
  size_t grank = -1;
  //static bool isInited = false;
  virtual bool Init(int argc, char* argv[]){
     //static bool isInited=false;   
     printf("EmptyEngine init called\n"); 
     printf("Env var== %s \n",getenv("CCL_PM_TYPE"));
     //if(isInited) {return true;}
     
     // Just for trace. 
     void* callstack[128];
     int i, frames = backtrace(callstack, 128);
     char** strs = backtrace_symbols(callstack, frames);
     for (i = 0; i < frames; ++i) {
         printf("%s\n", strs[i]);
     }
     free(strs);
     // Just for trace.     

     ccl_status_t status = ccl_init();
     if(status != ccl_status_success) {
           printf("Init failed....\n");
     }else{
          printf("Init success....\n");
     } 
     
     size_t size;
     size_t rank;
     //ccl_status_t status = ccl_init();
     //     //if(status != ccl_status_success) printf("Init failed....");
     //
      ccl_get_comm_rank(NULL, &grank);
     //ccl_get_comm_size(NULL, &size);
     
     //coll_attr.prologue_fn = NULL;
    // coll_attr.epilogue_fn = NULL;
    // coll_attr.reduction_fn = NULL;
    // coll_attr.priority = 0;
    // coll_attr.synchronous = 0;
    // coll_attr.match_id = "tensor_name";
     //coll_attr.to_cache = 0;
     //coll_attr.vector_buf = 0;     
     
     isInited = true;
     printf("EmptyEngine:: CCL_Init called......rank=%d\n",grank);
     return true;
  }
  // shutdown the engine
  virtual bool Shutdown(void){
     //Bug reported...we should be able to finalize
    ccl_finalize();
    printf("EmptyEngine:: CCL_Finalize from Shutdown called......\n");
    return true;
  }

 virtual bool Finalize() {
   ccl_finalize();
   printf("EmptyEngine:: CCL_Finalize called......\n");
   return true;
 }

 virtual void Allgather(void *sendrecvbuf,
                         size_t total_size,
                         size_t slice_begin,
                         size_t slice_end,
                         size_t size_prev_slice,
                         const char* _file = _FILE,
                         const int _line = _LINE,
                         const char* _caller = _CALLER){

    printf("EmptyEngine:: Allgather is called at EmptyEngine....\n");

} 


 void Allreduce(void *sendrecvbuf_,
                         size_t type_nbytes,
                         size_t count,
                         ReduceFunction reducer,
                         PreprocFunction prepare_fun,
                         void *prepare_arg,
                         const char* _file,
                         const int _line,
                         const char* _caller) {
     if (prepare_fun != NULL) prepare_fun(prepare_arg);
     if(!isInited){
      printf("Not initialized yet to call allreduce.........\n");
     }
     printf("type_nbytes................... %zu \n", type_nbytes );
      
      ccl_coll_attr_t coll_attr;
      ccl_request_t request;
      memset(&coll_attr, 0,sizeof(ccl_coll_attr_t));

     coll_attr.reduction_fn = do_reduction_sum2;     
     ccl_datatype_t type=ccl_dtype_float;
     if(type_nbytes%8 == 0)
     {
        type = ccl_dtype_bfp16;
     } if(type_nbytes%4 == 0){
        type = ccl_dtype_int;
      }else{
        type = ccl_dtype_char;
     }

     ccl_allreduce(sendrecvbuf_, sendrecvbuf_, count, type, ccl_reduction_custom, &coll_attr, NULL, NULL, &request);
     ccl_wait(request);
     printf("EmptyEngine:: Allreduce is finished at EmptyEngine....\n");
  }

  virtual void Broadcast(void *sendrecvbuf_, size_t size, int root,
                          const char* _file, const int _line, const char* _caller) {
    printf("EmptyEngine: Broad cast called\n");
     ccl_request_t request;
     ccl_datatype_t type=ccl_dtype_float;
     if(size%8 == 0)
     {
        type = ccl_dtype_bfp16;
     } if(size%4 == 0){
        type = ccl_dtype_int;
      }else{
        type = ccl_dtype_char;
     }
    ccl_bcast(sendrecvbuf_, size, type, root, NULL, NULL, NULL, &request);
    ccl_wait(request);
  }

  void Allreduce_(void *sendrecvbuf,
                size_t type_nbytes,
                size_t count,
                IEngine::ReduceFunction red,
                mpi::DataType dtype,
                mpi::OpType op,
                IEngine::PreprocFunction prepare_fun,
                void *prepare_arg,
                const char* _file,
                const int _line,
                const char* _caller) {
    if (prepare_fun != NULL) prepare_fun(prepare_arg);
  } 
 
  virtual void InitAfterException(void) {
    utils::Error("EmptyEngine is not fault tolerant");
  }
  virtual int LoadCheckPoint(Serializable *global_model,
                             Serializable *local_model = NULL) {
    return 0;
  }
  virtual void CheckPoint(const Serializable *global_model,
                          const Serializable *local_model = NULL) {
    version_number += 1;
  }
  virtual void LazyCheckPoint(const Serializable *global_model) {
    version_number += 1;
  }
  virtual int VersionNumber(void) const {
    return version_number;
  }
  virtual int GetRingPrevRank(void) const {

    return 0;
  }
  /*! \brief get rank of current node */
  virtual int GetRank(void) const {
    
    size_t rank;
    printf("GetRank called.....\n"); 
    
    //ccl_comm_create(&comm, &comm_attr);
    ccl_get_comm_rank(NULL, &rank);
    // auto stream = environment::instance().create_stream();
    // auto comm = environment::instance().create_communicator();
    // int rank = comm->rank();
     //size = comm->size();
    return rank;
  }
  /*! \brief get total number of */
  virtual int GetWorldSize(void) const {
    size_t size;
    ccl_get_comm_size(NULL, &size);
    //auto stream = environment::instance().create_stream();
     
    // auto comm = environment::instance().create_communicator();
    // int size = comm->size();
     return size;
  }
  /*! \brief whether it is distributed */
  virtual bool IsDistributed(void) const {
    return true;
  }
  /*! \brief get the host name of current node */
  virtual std::string GetHost(void) const {
    return std::string("");
  }
  virtual void TrackerPrint(const std::string &msg) {
    // simply print information into the tracker
    utils::Printf("%s", msg.c_str());
  }
  
  // singleton sync manager


 private:
  int version_number;
};

}  // namespace engine
}  // namespace rabit
