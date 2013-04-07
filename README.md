SylIcons - 
=======================

Requirement
-----------

* Sylpheed 3.2.0 or later
* User agent icons (use external resources such as DispMUA for example)

Sylicons depends "messageview-show" signal which is implemented since Sylpheed
3.2.0 or later.

How to Install
--------------

```
% git clone git://github.com/kenhys/sylpheed-icons.git
% cd sylpheed-icons
% ./autogen.sh
% ./configure --sylpheed-build-dir=/path/to/sylpheed_source_dir
% make
% sudo make install
```

Sylicons does not contain any user agent icons, so download external
user agent icons such as DispMUA, then put it them under $HOME/.sylpheed-2.0/plugins/sylicons.

You can download DispMUA from here.

http://www.juergen-ernst.de/addons/dispmua.html


How to use
----------



