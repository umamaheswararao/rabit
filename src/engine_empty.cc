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
#include "engine_empty.h"

namespace rabit {

namespace utils {
  bool STOP_PROCESS_ON_ERROR = true;
}

namespace engine {
/*! \brief EmptyEngine */

// singleton sync manager
EmptyEngine manager;

/*! \brief intiialize the synchronization module */
bool Init(int argc, char *argv[]) {
  printf("Dummy implementation....doing nothing.............................Engine_Empty.cc");
  return true;
}
/*! \brief finalize syncrhonization module */
bool Shutdown(void) {
  return true;
}

bool Finalize() {
  return true;
}

/*! \brief singleton method to get engine */
IEngine *GetEngine(void) {
  return &manager;
}
// perform in-place allreduce, on sendrecvbuf

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
  printf("Inplace allreduce called ...............................\n");
}

// code for reduce handle
ReduceHandle::ReduceHandle(void) : handle_(NULL), htype_(NULL) {
}
ReduceHandle::~ReduceHandle(void) {}

int ReduceHandle::TypeSize(const MPI::Datatype &dtype) {
  return 0;
}
void ReduceHandle::Init(IEngine::ReduceFunction redfunc, size_t type_nbytes) {}
void ReduceHandle::Allreduce(void *sendrecvbuf,
                             size_t type_nbytes, size_t count,
                             IEngine::PreprocFunction prepare_fun,
                             void *prepare_arg,
                             const char* _file,
                             const int _line,
                             const char* _caller) {
  if (prepare_fun != NULL) prepare_fun(prepare_arg);
}
}  // namespace engine
}  // namespace rabit
