
#include "class_loader.h"

namespace apollo {
namespace cyber {
namespace class_loader {
ClassLoader::ClassLoader(const std::string& library_path)
    : library_path_(library_path),
      loadlib_ref_count_(0),
      classobj_ref_count_(0) {
  LoadLibrary();
}

ClassLoader::~ClassLoader() { UnloadLibrary(); }

bool ClassLoader::IsLibraryLoaded() {
  return utility::IsLibraryLoaded(library_path_, this);
}

bool ClassLoader::LoadLibrary() {
  std::lock_guard<std::mutex> lck(loadlib_ref_count_mutex_);
  loadlib_ref_count_ = loadlib_ref_count_ + 1;
  return utility::LoadLibrary(library_path_, this);
}

int ClassLoader::UnloadLibrary() {
  std::lock_guard<std::mutex> lckLib(loadlib_ref_count_mutex_);
  std::lock_guard<std::mutex> lckObj(classobj_ref_count_mutex_);

  if (classobj_ref_count_ > 0) {
    AINFO << "There are still classobjs have not been deleted, "
             "classobj_ref_count_: "
          << classobj_ref_count_;
  } else {
    loadlib_ref_count_ = loadlib_ref_count_ - 1;
    if (loadlib_ref_count_ == 0) {
      utility::UnloadLibrary(library_path_, this);
    } else {
      if (loadlib_ref_count_ < 0) {
        loadlib_ref_count_ = 0;
      }
    }
  }
  return loadlib_ref_count_;
}

const std::string ClassLoader::GetLibraryPath() const { return library_path_; }

}  // namespace class_loader
}  // namespace cyber
}  // namespace apollo
