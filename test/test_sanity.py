#!/usr/bin/env python

## $Id$

import urllib, random
from boinc import *

# test makes sure that testing framework is sane

def read_url(url, quiet=False):
    '''return 1 line from url'''
    verbose_echo(2, "   reading url: "+url)
    try:
        return urllib.URLopener().open(url).readline().strip()
    except IOError, e:
        if not quiet:
            error("couldn't access url: %s %s" % (url, e))
        return ''

if __name__ == '__main__':
    test_msg("framework sanity");

    verbose_echo(1, "Checking executables")
    check_core_client_executable()
    check_app_executable("upper_case")
    check_app_executable("concat")
    check_app_executable("1sec")

    verbose_echo(1, "Checking directories")
    for d in ['KEY_DIR', 'PROJECTS_DIR',
              'CGI_DIR', 'HTML_DIR', 'HOSTS_DIR']:
        dir = globals()[d]
        if not os.path.isdir(dir):
            error("%s doesn't exist: %s" % (d, dir))

    magic = "Foo %x Bar" % random.randint(0,2**16)

    html_path = os.path.join(HTML_DIR, 'test_sanity.txt')
    html_url  = os.path.join(HTML_URL, 'test_sanity.txt')
    html_proxy_url = proxerize(html_url)
    cgi_path  = os.path.join(CGI_DIR,  'test_sanity_cgi')
    cgi_url   = os.path.join(CGI_URL,  'test_sanity_cgi')

    verbose_echo(1, "Checking webserver setup: non-cgi")
    print >>open(html_path,'w'), magic
    if read_url(html_url) != magic:
        error("couldn't access a file I just wrote: "+html_path+"\n  using url: "+html_url)

    verbose_echo(1, "Checking proxy setup")
    if read_url(html_proxy_url, quiet=True):
        error("Another proxy already running")
    else:
        proxy = Proxy('')
        if read_url(html_proxy_url) != magic:
            error("couldn't access file using proxy url: "+html_proxy_url)
        else:
            proxy.stop()

            proxy = Proxy('exit 1 if $nconnections < 2; if_done_kill(); if_done_ping();')
            if read_url(html_proxy_url, quiet=True):
                error("Proxy should have closed connection #1")
                if read_url(html_proxy_url) != magic:
                    error("Proxy should have allowed connection #2")
        proxy.stop()

    os.unlink(html_path)

    verbose_echo(1, "Checking webserver setup: cgi")
    print >>open(cgi_path,'w'), '''#!/bin/sh
echo "Content-Type: text/plain"
echo ""
echo "%s"
''' % magic
    os.chmod(cgi_path, 0755)

    if read_url(cgi_url) != magic:
        error("couldn't access a cgi file I just wrote: "+cgi_path+"\n  using url: "+cgi_url)

    os.unlink(cgi_path)
