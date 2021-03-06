  $ rbd resize --snap=snap1 img
  error: snapname specified for a command that doesn't use it
  usage: rbd [-n <auth user>] [OPTIONS] <cmd> ...
  where 'pool' is a rados pool name (default is 'rbd') and 'cmd' is one of:
    <ls | list> [pool-name]                   list rbd images
    info <--snap=name> [image-name]           show information about image size,
                                              striping, etc.
    create [image-name]                       create an empty image (requires size
                                              param)
    resize [image-name]                       resize (expand or contract) image
                                              (requires size param)
    rm [image-name]                           delete an image
    export <--snap=name> [image-name] [path]  export image to file
    import [path] [dst-image]                 import image from file (dest defaults
                                              as the filename part of file)
    <cp | copy> <--snap=name> [src] [dest]    copy src image to dest
    <mv | rename> [src] [dest]                rename src image to dest
    snap ls [image-name]                      dump list of image snapshots
    snap create <--snap=name> [image-name]    create a snapshot
    snap rollback <--snap=name> [image-name]  rollback image head to snapshot
    snap rm <--snap=name> [image-name]        deletes a snapshot
    watch [image-name]                        watch events on image
    map [image-name]                          map the image to a block device
                                              using the kernel
    unmap [device]                            unmap a rbd device that was
                                              mapped by the kernel
  
  Other input options:
    -p, --pool <pool>            source pool name
    --image <image-name>         image name
    --dest <name>                destination [pool and] image name
    --snap <snapname>            specify snapshot name
    --dest-pool <name>           destination pool name
    --path <path-name>           path name for import/export (if not specified)
    --size <size in MB>          size parameter for create and resize commands
  
  For the map command:
    --user <username>            rados user to authenticate as
    --secret <path>              file containing secret key for use with authx
  [1]
  $ rbd resize img@snap
  error: snapname specified for a command that doesn't use it
  usage: rbd [-n <auth user>] [OPTIONS] <cmd> ...
  where 'pool' is a rados pool name (default is 'rbd') and 'cmd' is one of:
    <ls | list> [pool-name]                   list rbd images
    info <--snap=name> [image-name]           show information about image size,
                                              striping, etc.
    create [image-name]                       create an empty image (requires size
                                              param)
    resize [image-name]                       resize (expand or contract) image
                                              (requires size param)
    rm [image-name]                           delete an image
    export <--snap=name> [image-name] [path]  export image to file
    import [path] [dst-image]                 import image from file (dest defaults
                                              as the filename part of file)
    <cp | copy> <--snap=name> [src] [dest]    copy src image to dest
    <mv | rename> [src] [dest]                rename src image to dest
    snap ls [image-name]                      dump list of image snapshots
    snap create <--snap=name> [image-name]    create a snapshot
    snap rollback <--snap=name> [image-name]  rollback image head to snapshot
    snap rm <--snap=name> [image-name]        deletes a snapshot
    watch [image-name]                        watch events on image
    map [image-name]                          map the image to a block device
                                              using the kernel
    unmap [device]                            unmap a rbd device that was
                                              mapped by the kernel
  
  Other input options:
    -p, --pool <pool>            source pool name
    --image <image-name>         image name
    --dest <name>                destination [pool and] image name
    --snap <snapname>            specify snapshot name
    --dest-pool <name>           destination pool name
    --path <path-name>           path name for import/export (if not specified)
    --size <size in MB>          size parameter for create and resize commands
  
  For the map command:
    --user <username>            rados user to authenticate as
    --secret <path>              file containing secret key for use with authx
  [1]
  $ rbd import --snap=snap1 /bin/ls ls
  error: snapname specified for a command that doesn't use it
  usage: rbd [-n <auth user>] [OPTIONS] <cmd> ...
  where 'pool' is a rados pool name (default is 'rbd') and 'cmd' is one of:
    <ls | list> [pool-name]                   list rbd images
    info <--snap=name> [image-name]           show information about image size,
                                              striping, etc.
    create [image-name]                       create an empty image (requires size
                                              param)
    resize [image-name]                       resize (expand or contract) image
                                              (requires size param)
    rm [image-name]                           delete an image
    export <--snap=name> [image-name] [path]  export image to file
    import [path] [dst-image]                 import image from file (dest defaults
                                              as the filename part of file)
    <cp | copy> <--snap=name> [src] [dest]    copy src image to dest
    <mv | rename> [src] [dest]                rename src image to dest
    snap ls [image-name]                      dump list of image snapshots
    snap create <--snap=name> [image-name]    create a snapshot
    snap rollback <--snap=name> [image-name]  rollback image head to snapshot
    snap rm <--snap=name> [image-name]        deletes a snapshot
    watch [image-name]                        watch events on image
    map [image-name]                          map the image to a block device
                                              using the kernel
    unmap [device]                            unmap a rbd device that was
                                              mapped by the kernel
  
  Other input options:
    -p, --pool <pool>            source pool name
    --image <image-name>         image name
    --dest <name>                destination [pool and] image name
    --snap <snapname>            specify snapshot name
    --dest-pool <name>           destination pool name
    --path <path-name>           path name for import/export (if not specified)
    --size <size in MB>          size parameter for create and resize commands
  
  For the map command:
    --user <username>            rados user to authenticate as
    --secret <path>              file containing secret key for use with authx
  [1]
  $ rbd create --snap=snap img
  error: snapname specified for a command that doesn't use it
  usage: rbd [-n <auth user>] [OPTIONS] <cmd> ...
  where 'pool' is a rados pool name (default is 'rbd') and 'cmd' is one of:
    <ls | list> [pool-name]                   list rbd images
    info <--snap=name> [image-name]           show information about image size,
                                              striping, etc.
    create [image-name]                       create an empty image (requires size
                                              param)
    resize [image-name]                       resize (expand or contract) image
                                              (requires size param)
    rm [image-name]                           delete an image
    export <--snap=name> [image-name] [path]  export image to file
    import [path] [dst-image]                 import image from file (dest defaults
                                              as the filename part of file)
    <cp | copy> <--snap=name> [src] [dest]    copy src image to dest
    <mv | rename> [src] [dest]                rename src image to dest
    snap ls [image-name]                      dump list of image snapshots
    snap create <--snap=name> [image-name]    create a snapshot
    snap rollback <--snap=name> [image-name]  rollback image head to snapshot
    snap rm <--snap=name> [image-name]        deletes a snapshot
    watch [image-name]                        watch events on image
    map [image-name]                          map the image to a block device
                                              using the kernel
    unmap [device]                            unmap a rbd device that was
                                              mapped by the kernel
  
  Other input options:
    -p, --pool <pool>            source pool name
    --image <image-name>         image name
    --dest <name>                destination [pool and] image name
    --snap <snapname>            specify snapshot name
    --dest-pool <name>           destination pool name
    --path <path-name>           path name for import/export (if not specified)
    --size <size in MB>          size parameter for create and resize commands
  
  For the map command:
    --user <username>            rados user to authenticate as
    --secret <path>              file containing secret key for use with authx
  [1]
  $ rbd rm --snap=snap img
  error: snapname specified for a command that doesn't use it
  usage: rbd [-n <auth user>] [OPTIONS] <cmd> ...
  where 'pool' is a rados pool name (default is 'rbd') and 'cmd' is one of:
    <ls | list> [pool-name]                   list rbd images
    info <--snap=name> [image-name]           show information about image size,
                                              striping, etc.
    create [image-name]                       create an empty image (requires size
                                              param)
    resize [image-name]                       resize (expand or contract) image
                                              (requires size param)
    rm [image-name]                           delete an image
    export <--snap=name> [image-name] [path]  export image to file
    import [path] [dst-image]                 import image from file (dest defaults
                                              as the filename part of file)
    <cp | copy> <--snap=name> [src] [dest]    copy src image to dest
    <mv | rename> [src] [dest]                rename src image to dest
    snap ls [image-name]                      dump list of image snapshots
    snap create <--snap=name> [image-name]    create a snapshot
    snap rollback <--snap=name> [image-name]  rollback image head to snapshot
    snap rm <--snap=name> [image-name]        deletes a snapshot
    watch [image-name]                        watch events on image
    map [image-name]                          map the image to a block device
                                              using the kernel
    unmap [device]                            unmap a rbd device that was
                                              mapped by the kernel
  
  Other input options:
    -p, --pool <pool>            source pool name
    --image <image-name>         image name
    --dest <name>                destination [pool and] image name
    --snap <snapname>            specify snapshot name
    --dest-pool <name>           destination pool name
    --path <path-name>           path name for import/export (if not specified)
    --size <size in MB>          size parameter for create and resize commands
  
  For the map command:
    --user <username>            rados user to authenticate as
    --secret <path>              file containing secret key for use with authx
  [1]
  $ rbd rename --snap=snap img
  error: snapname specified for a command that doesn't use it
  usage: rbd [-n <auth user>] [OPTIONS] <cmd> ...
  where 'pool' is a rados pool name (default is 'rbd') and 'cmd' is one of:
    <ls | list> [pool-name]                   list rbd images
    info <--snap=name> [image-name]           show information about image size,
                                              striping, etc.
    create [image-name]                       create an empty image (requires size
                                              param)
    resize [image-name]                       resize (expand or contract) image
                                              (requires size param)
    rm [image-name]                           delete an image
    export <--snap=name> [image-name] [path]  export image to file
    import [path] [dst-image]                 import image from file (dest defaults
                                              as the filename part of file)
    <cp | copy> <--snap=name> [src] [dest]    copy src image to dest
    <mv | rename> [src] [dest]                rename src image to dest
    snap ls [image-name]                      dump list of image snapshots
    snap create <--snap=name> [image-name]    create a snapshot
    snap rollback <--snap=name> [image-name]  rollback image head to snapshot
    snap rm <--snap=name> [image-name]        deletes a snapshot
    watch [image-name]                        watch events on image
    map [image-name]                          map the image to a block device
                                              using the kernel
    unmap [device]                            unmap a rbd device that was
                                              mapped by the kernel
  
  Other input options:
    -p, --pool <pool>            source pool name
    --image <image-name>         image name
    --dest <name>                destination [pool and] image name
    --snap <snapname>            specify snapshot name
    --dest-pool <name>           destination pool name
    --path <path-name>           path name for import/export (if not specified)
    --size <size in MB>          size parameter for create and resize commands
  
  For the map command:
    --user <username>            rados user to authenticate as
    --secret <path>              file containing secret key for use with authx
  [1]
  $ rbd ls --snap=snap rbd
  error: snapname specified for a command that doesn't use it
  usage: rbd [-n <auth user>] [OPTIONS] <cmd> ...
  where 'pool' is a rados pool name (default is 'rbd') and 'cmd' is one of:
    <ls | list> [pool-name]                   list rbd images
    info <--snap=name> [image-name]           show information about image size,
                                              striping, etc.
    create [image-name]                       create an empty image (requires size
                                              param)
    resize [image-name]                       resize (expand or contract) image
                                              (requires size param)
    rm [image-name]                           delete an image
    export <--snap=name> [image-name] [path]  export image to file
    import [path] [dst-image]                 import image from file (dest defaults
                                              as the filename part of file)
    <cp | copy> <--snap=name> [src] [dest]    copy src image to dest
    <mv | rename> [src] [dest]                rename src image to dest
    snap ls [image-name]                      dump list of image snapshots
    snap create <--snap=name> [image-name]    create a snapshot
    snap rollback <--snap=name> [image-name]  rollback image head to snapshot
    snap rm <--snap=name> [image-name]        deletes a snapshot
    watch [image-name]                        watch events on image
    map [image-name]                          map the image to a block device
                                              using the kernel
    unmap [device]                            unmap a rbd device that was
                                              mapped by the kernel
  
  Other input options:
    -p, --pool <pool>            source pool name
    --image <image-name>         image name
    --dest <name>                destination [pool and] image name
    --snap <snapname>            specify snapshot name
    --dest-pool <name>           destination pool name
    --path <path-name>           path name for import/export (if not specified)
    --size <size in MB>          size parameter for create and resize commands
  
  For the map command:
    --user <username>            rados user to authenticate as
    --secret <path>              file containing secret key for use with authx
  [1]
  $ rbd snap ls --snap=snap img
  error: snapname specified for a command that doesn't use it
  usage: rbd [-n <auth user>] [OPTIONS] <cmd> ...
  where 'pool' is a rados pool name (default is 'rbd') and 'cmd' is one of:
    <ls | list> [pool-name]                   list rbd images
    info <--snap=name> [image-name]           show information about image size,
                                              striping, etc.
    create [image-name]                       create an empty image (requires size
                                              param)
    resize [image-name]                       resize (expand or contract) image
                                              (requires size param)
    rm [image-name]                           delete an image
    export <--snap=name> [image-name] [path]  export image to file
    import [path] [dst-image]                 import image from file (dest defaults
                                              as the filename part of file)
    <cp | copy> <--snap=name> [src] [dest]    copy src image to dest
    <mv | rename> [src] [dest]                rename src image to dest
    snap ls [image-name]                      dump list of image snapshots
    snap create <--snap=name> [image-name]    create a snapshot
    snap rollback <--snap=name> [image-name]  rollback image head to snapshot
    snap rm <--snap=name> [image-name]        deletes a snapshot
    watch [image-name]                        watch events on image
    map [image-name]                          map the image to a block device
                                              using the kernel
    unmap [device]                            unmap a rbd device that was
                                              mapped by the kernel
  
  Other input options:
    -p, --pool <pool>            source pool name
    --image <image-name>         image name
    --dest <name>                destination [pool and] image name
    --snap <snapname>            specify snapshot name
    --dest-pool <name>           destination pool name
    --path <path-name>           path name for import/export (if not specified)
    --size <size in MB>          size parameter for create and resize commands
  
  For the map command:
    --user <username>            rados user to authenticate as
    --secret <path>              file containing secret key for use with authx
  [1]
  $ rbd watch --snap=snap img
  error: snapname specified for a command that doesn't use it
  usage: rbd [-n <auth user>] [OPTIONS] <cmd> ...
  where 'pool' is a rados pool name (default is 'rbd') and 'cmd' is one of:
    <ls | list> [pool-name]                   list rbd images
    info <--snap=name> [image-name]           show information about image size,
                                              striping, etc.
    create [image-name]                       create an empty image (requires size
                                              param)
    resize [image-name]                       resize (expand or contract) image
                                              (requires size param)
    rm [image-name]                           delete an image
    export <--snap=name> [image-name] [path]  export image to file
    import [path] [dst-image]                 import image from file (dest defaults
                                              as the filename part of file)
    <cp | copy> <--snap=name> [src] [dest]    copy src image to dest
    <mv | rename> [src] [dest]                rename src image to dest
    snap ls [image-name]                      dump list of image snapshots
    snap create <--snap=name> [image-name]    create a snapshot
    snap rollback <--snap=name> [image-name]  rollback image head to snapshot
    snap rm <--snap=name> [image-name]        deletes a snapshot
    watch [image-name]                        watch events on image
    map [image-name]                          map the image to a block device
                                              using the kernel
    unmap [device]                            unmap a rbd device that was
                                              mapped by the kernel
  
  Other input options:
    -p, --pool <pool>            source pool name
    --image <image-name>         image name
    --dest <name>                destination [pool and] image name
    --snap <snapname>            specify snapshot name
    --dest-pool <name>           destination pool name
    --path <path-name>           path name for import/export (if not specified)
    --size <size in MB>          size parameter for create and resize commands
  
  For the map command:
    --user <username>            rados user to authenticate as
    --secret <path>              file containing secret key for use with authx
  [1]
