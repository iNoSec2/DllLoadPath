# DllLoadPath

Showcasing two different techniques for changing DLL load order by using undocumented APIs.
These are not novel techniques but I never saw them documented anywhere.

 The proper signatures for RtlCreateUserProcess and RtlCreateProcessParameters are:
 
 ```cpp
RtlCreateUserProcess(
  IN PUNICODE_STRING      ImagePath,
  IN ULONG                ObjectAttributes,
  IN OUT PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
  IN PSECURITY_DESCRIPTOR ProcessSecurityDescriptor OPTIONAL,
  IN PSECURITY_DESCRIPTOR ThreadSecurityDescriptor OPTIONAL,
  IN HANDLE               ParentProcess,
  IN BOOLEAN              InheritHandles,
  IN HANDLE               DebugPort OPTIONAL,
  IN HANDLE               ExceptionPort OPTIONAL,
  OUT PRTL_USER_PROCESS_INFORMATION ProcessInformation );
  
RtlCreateProcessParameters(
    PRTL_USER_PROCESS_PARAMETERS *pProcessParameters,
    PUNICODE_STRING ImagePathName,
    PUNICODE_STRING DllPath,
    PUNICODE_STRING CurrentDirectory,
    PUNICODE_STRING CommandLine,
    PVOID Environment,
    PUNICODE_STRING WindowTitle,
    PUNICODE_STRING DesktopInfo,
    PUNICODE_STRING ShellInfo,
    PUNICODE_STRING RuntimeData
    );
  ```
  
By changing some values in RtlCreateProcessParameters we can achieve interesting behavior.

# Changing the working DLL load order directory entirely

Writing this one up for people was inspired by this [tweet](https://twitter.com/Octoberfest73/status/1642165975805050881), thank you for reminding me of it :)

![image](https://user-images.githubusercontent.com/74931194/230739781-bef31578-3812-44f1-944f-e757d1697454.png)

```cpp
   RtlInitUnicodeString(&ImagePathName, LR"(\??\C:\Windows\System32\WerFault.exe)");
   // Modifying CommandLine will cause DLLs to start being loaded from a different path
  ...
  status = RtlCreateProcessParameters(&UserProcessParam, &CommandLine, &CommandLine, NULL, &CommandLine, NULL, NULL, NULL, NULL, NULL);
```

# Changing the second search path of the DLL search order

This one will will cause the application to still look into it's own directory before the one you choose, but it may come useful in some cases.

![image](https://user-images.githubusercontent.com/74931194/230739994-c927101d-aa32-4eba-b0a9-91f19ce86afa.png)

```cpp
  // DllPath will cause the second path in the load order to be changed
	 RtlInitUnicodeString(&DllPath, LR"(C:/ProgramData/)");
  ...
  status = RtlCreateProcessParameters(&UserProcessParam, &ImagePathName, &DllPath, NULL, &CommandLine, NULL, NULL, NULL, NULL, NULL);
 ```
 
Many thanks to [this post](http://www.rohitab.com/discuss/topic/41379-running-native-applications-with-rtlcreateuserprocess/) for helping me work out some parts.
