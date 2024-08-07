# CrowArmor : Linux Driver

![CrowArmor](assets/crowarmor.png)

# What is CrowArmor ?

CrowArmor is a driver for Linux aimed at system security, we use [LSM](https://en.wikipedia.org/wiki/Linux_Security_Modules) interfaces to improve kernel security, providing support for [MalDec-EDR](https://github.com/maldeclabs/MalDec-EDR), code documentation and how to proceed with installation [Documentation](documentation)

# Development Process

The standard practice is to have the latest stable production version available for customers in the main and tagged branch. The test branch serves as a mirror of the development branch, subjecting it to a battery of tests and quality assurance (QA) processes. Meanwhile, the development (dev) branch is dedicated to ongoing project development, enhancements, and adjustments.

```
                +-----------+
                | feature1  |
                +-----------+
                     |
                +-----------+
                | feature3  |
                +-----------+
                     |
                +-----------+
                | feature2  |
                +-----------+
                     |
                +-----------+      +-----------+      +-----------+
                |    dev    | ---> |    test   | ---> |   main    |
                +-----------+      +-----------+      +-----------+
                                                            |
                                                +--------------------------+
                                                |            |             |
                                            +-----+      +-----+       +-----+
                                            |1.0.0|      |2.0.0|       | ... |
                                            +-----+      +-----+       +-----+
```
# Testing

You need to test all components of MalDec-EDR, if possible detail the task of the components you tested, which paths you took and how we can perform the tests, if possible create a script of how the tests for your task work, more than 1 developer can carry out the review.

## Automated Testing

Every modification to the code, no matter how small, should ideally be accompanied by thorough unit tests. This practice is crucial for detecting potential errors introduced by other developers. The presence of unit tests serves as a safeguard, ensuring that any unintended changes are promptly identified and addressed.

## Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the code. This is especially important for large or high-risk changes. It is useful to add a test plan to the pull request description if testing the changes is not straightforward.

## Features

- Communicates with MalDec EDR
- Modifies the Syscall Table and hooks it
- Monitors if the Syscall Table has been modified
- Restores the Syscall if it has been modified by an unknown driver
- Monitors the CPU Control Registers and restores them if they are modified

## Tests carried out

- [X] Distro Debian-Based
    - Ubuntu
    - Debian
- [x] Distro Arch-Based
    - Arch Linux
    - Artix
- [x] Kernel Version 6.0.0
    - 6.9.7-arch1-1
    - 6.8.0-36-generic
    - 6.8.0-35-generic
    - 6.5.0-060500-generic
    - 6.3.0-060300-generic
    - 6.2.0-060200-generic
    - 6.1.0-060-100-generic
    - 6.0.0-060000
- [x] Kernel Version 5.0.0
    - 5.0.5-050005-generic
- [x] Kernel Version 4.0.0
