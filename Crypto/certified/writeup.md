# Writeup for Certified

## Solution

Flag is in the OID 1.3.6.1.4.1.56337.1. Just clean out the first few bits
which OpenSSL parses (which is something to learn about extensions,
they're very easy to end up with a corrupted value) and then base64 decode the value.

```bash
openssl -in cert.pem -noout -text
```

```bash
Certificate:
    Data:
        Version: 3 (0x2)
        Serial Number:
            7d:13:aa:01:b8:d3:2c:95:68:64:ee:1a:f5:5a:2b:5a:ce:06:08:ac
        Signature Algorithm: sha256WithRSAEncryption
        Issuer: C = US, ST = FL, L = Orlando, O = CTF, OU = CTF, CN = CTF
        Validity
            Not Before: Oct 10 06:02:49 2024 GMT
            Not After : Nov  9 06:02:49 2024 GMT
        Subject: C = US, ST = FL, L = Orlando, O = CTF, OU = CTF, CN = CTF
        Subject Public Key Info:
            Public Key Algorithm: rsaEncryption
                Public-Key: (2048 bit)
                Modulus:
                    00:a2:05:4a:0b:4f:e8:c6:b2:12:fd:57:b5:f3:d5:
                    e0:f2:c6:cf:4b:da:bc:64:a1:32:69:f9:90:49:c6:
                    e0:02:81:df:5b:d6:18:9e:3e:ef:14:c4:41:b3:14:
                    62:cf:24:44:e9:77:9c:bf:f9:76:cd:49:8a:31:b3:
                    ca:6d:55:50:cd:13:44:ba:d8:3e:e2:24:7f:64:28:
                    d6:2b:6c:53:bf:75:ea:3e:c8:39:cb:d1:c6:62:d4:
                    ab:66:91:06:66:d4:f9:77:fc:33:1d:e5:cc:4a:fd:
                    77:db:65:96:7e:9b:81:e2:80:c9:57:06:9c:57:03:
                    0d:89:22:da:15:35:ae:0a:55:fd:9d:1e:08:90:f4:
                    98:21:16:ce:b1:30:28:64:5d:aa:e5:ea:17:02:cc:
                    16:29:f7:eb:6b:67:54:0c:fd:89:ba:06:c3:4c:1f:
                    ef:f3:56:24:1a:e5:39:23:dd:da:e6:d4:81:07:38:
                    10:c8:41:78:32:b0:2f:8d:cc:6e:37:8e:92:59:d3:
                    53:49:5d:87:9b:23:1f:29:f7:2d:f4:85:3a:3d:e0:
                    a9:7b:6a:f8:e5:5f:1a:90:9e:41:13:fe:c1:89:06:
                    5f:91:59:8d:ef:21:a3:97:80:ec:e0:c7:8d:02:03:
                    42:19:97:75:75:af:c3:a4:6c:bb:3d:6d:ce:2d:b2:
                    8e:83
                Exponent: 65537 (0x10001)
        X509v3 extensions:
            1.3.6.1.4.1.56337.1: 
                .4c3Vue2IzdF91X2QxZG50X2tuMHdfYjB1dF9vMWRfbXNncyF9Cg==
            X509v3 Subject Key Identifier: 
                7F:38:8E:1B:BA:DA:D3:7D:2A:34:B6:43:53:5A:54:8A:FD:0A:4A:C7
            X509v3 Authority Key Identifier: 
                DirName:/C=US/ST=FL/L=Orlando/O=CTF/OU=CTF/CN=CTF
                serial:5A:A7:0F:0A:03:19:4D:0F:BB:90:AB:2D:63:D6:8C:5C:E0:FE:52:BF
    Signature Algorithm: sha256WithRSAEncryption
    Signature Value:
        ba:99:dc:08:bc:bc:4b:b3:79:95:8e:48:4d:27:32:17:b2:02:
        05:43:72:ae:f4:2e:5f:cf:d2:9f:e7:ee:97:5b:12:7a:8d:68:
        00:c2:80:50:d2:c0:7e:35:b6:9c:83:10:81:70:95:17:b2:ec:
        38:3b:10:d4:86:f5:1d:09:65:8f:91:52:2d:4b:f9:68:a2:90:
        9a:5e:ab:80:4c:de:75:91:22:27:4a:97:3d:80:69:9f:15:87:
        59:69:92:97:c8:ea:79:aa:a6:d3:cd:53:ec:54:f6:d0:af:e5:
        c7:89:60:c3:db:a8:68:24:3e:03:17:1b:23:05:4d:11:44:eb:
        63:4e:19:46:b7:44:ab:28:e7:a4:7c:83:eb:ba:fd:4c:9b:7d:
        dc:52:78:ed:b1:de:af:cf:e9:e1:a3:f9:1d:c9:12:f9:7c:93:
        13:09:ba:2e:5d:9a:30:52:a0:58:2b:07:71:c6:af:cc:7e:3b:
        36:21:34:80:15:28:95:13:45:61:10:91:b2:76:bb:8a:b9:b7:
        4c:55:a5:72:de:66:a4:c0:7a:68:9c:01:cd:24:39:f1:47:75:
        8a:5b:3b:c4:a2:ca:23:c7:18:2e:fe:3c:e1:ec:76:7f:8c:a3:
        9f:8f:02:74:2f:19:0a:90:cc:da:9c:a3:7e:f1:84:fe:0f:a6:
        c8:77:6f:ae
Checking solution...
Cleaned Base64: c3Vue2IzdF91X2QxZG50X2tuMHdfYjB1dF9vMWRfbXNncyF9Cg==
Flag: sun{b3t_u_d1dnt_kn0w_b0ut_o1d_msgs!}
Flag is correct!
```
