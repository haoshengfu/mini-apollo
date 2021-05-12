
#include "class_factory.h"

#include "../class_loader.h"

namespace apollo {
namespace cyber {
namespace class_loader {
namespace utility {

AbstractClassFactoryBase::AbstractClassFactoryBase(
    const std::string& class_name, const std::string& base_class_name)
    : relative_library_path_(""),
      base_class_name_(base_class_name),
      class_name_(class_name) {}

AbstractClassFactoryBase::~AbstractClassFactoryBase() {}

void AbstractClassFactoryBase::SetRelativeLibraryPath(
    const std::string& library_path) {
  relative_library_path_ = library_path;
}

void AbstractClassFactoryBase::AddOwnedClassLoader(ClassLoader* loader) {
  if (std::find(relative_class_loaders_.begin(), relative_class_loaders_.end(),
                loader) == relative_class_loaders_.end()) {
    relative_class_loaders_.emplace_back(loader);
  }
}

void AbstractClassFactoryBase::RemoveOwnedClassLoader(
    const ClassLoader* loader) {
  std::vector<ClassLoader*>::iterator itr = std::find(
      relative_class_loaders_.begin(), relative_class_loaders_.end(), loader);
  if (itr != relative_class_loaders_.end()) {
    relative_class_loaders_.erase(itr);
  }
}

bool AbstractClassFactoryBase::IsOwnedBy(const ClassLoader* loader) {
  std::vector<ClassLoader*>::iterator itr = std::find(
      relative_class_loaders_.begin(), relative_class_loaders_.end(), loader);
  return (itr != relative_class_loaders_.end());
}

bool AbstractClassFactoryBase::IsOwnedByAnybody() {
  return (relative_class_loaders_.size() > 0);
}

std::vector<ClassLoader*> AbstractClassFactoryBase::GetRelativeClassLoaders() {
  return relative_class_loaders_;
}

const std::string AbstractClassFactoryBase::GetRelativeLibraryPath() const {
  return relative_library_path_;
}

const std::string AbstractClassFactoryBase::GetBaseClassName() const {
  return base_class_name_;
}

const std::string AbstractClassFactoryBase::GetClassName() const {
  return class_name_;
}

}  // namespace utility
}  // namespace class_loader
}  // namespace cyber
}  // namespace apollo
