#pragma once

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // ObjectAttributes

    struct ObjectAttributes : public OBJECT_ATTRIBUTES
    {
        ObjectAttributes(const UNICODE_STRING* objectName, PSECURITY_DESCRIPTOR securityDescriptor) : ObjectAttributes(objectName, OBJ_KERNEL_HANDLE, nullptr, securityDescriptor)
        {
        }

        ObjectAttributes(const UNICODE_STRING* objectName = nullptr, ULONG attributes = OBJ_KERNEL_HANDLE, HANDLE rootDirectory = nullptr, PSECURITY_DESCRIPTOR securityDescriptor = nullptr, PSECURITY_QUALITY_OF_SERVICE securityQualityOfService = nullptr)
        {
            Length = sizeof(OBJECT_ATTRIBUTES);
            ObjectName = const_cast<UNICODE_STRING*>(objectName);
            Attributes = attributes;
            RootDirectory = rootDirectory;
            SecurityDescriptor = securityDescriptor;
            SecurityQualityOfService = securityQualityOfService;
        }
    };
} // namespace
