Port Assignments
-----

Port assignment convention:

* SunshineCTF challenges: port = `yycnn`
  * `yy` = last 2 digits of competition year, so SunshineCTF 2024 is `24cnn`
  * `c` = category (0 = pwn, 1 = reversing, 2 = scripting, 3 = web, 4 = crypto,
    5 = misc/forensics/other, 6 = speedrun [i95], 7 = pegasus)
  * `nn` = challenge index within the category, starting at 01

Only challenges with a hosted, server-side component are listed below. Raw TCP
challenges are reached with `nc ctf.hackucf.org <port>`; HTTP(S) web challenges
are served behind nginx on a per-challenge subdomain
(`<slug>.ctf.hackucf.org`) and the container is bound to `127.0.0.1:<port>`.

| Challenge Name          | Category  | Author             | Directory                     | Host                                     | Port  | Connection                                      |
|-------------------------|-----------|--------------------|-------------------------------|------------------------------------------|------:|-------------------------------------------------|
| FlagShop                | Pwn       | alimuhammadsecured | Pwn/FlagShop                  | ctf.hackucf.org                          | 24001 | `nc ctf.hackucf.org 24001`                      |
| Secure Flag Terminal    | Pwn       | Oreomeister        | Pwn/Secure_Flag_Terminal      | ctf.hackucf.org                          | 24002 | `nc ctf.hackucf.org 24002`                      |
| Adventure on the High C | Pwn       | tj_oconnor         | Pwn/adventure-on-the-high-c   | ctf.hackucf.org                          | 24003 | `nc ctf.hackucf.org 24003`                      |
| Drone Adventure         | Pwn       | tj_oconnor         | Pwn/drone-adventure           | ctf.hackucf.org                          | 24004 | `nc ctf.hackucf.org 24004`                      |
| Welcome to the Jungle   | Pwn       | tj_oconnor         | Pwn/jungle-adventure          | ctf.hackucf.org                          | 24005 | `nc ctf.hackucf.org 24005`                      |
| Heap 1                  | Pwn       | guyinatuxedo       | Pwn/heap-01                   | ctf.hackucf.org                          | 24006 | `nc ctf.hackucf.org 24006`                      |
| Dungeon Keymaster       | Reversing | solarbonite        | Reversing/dungeon-keymaster   | dungeon-keymaster.ctf.hackucf.org   | 24101 | https://dungeon-keymaster.ctf.hackucf.org  |
| BotNet (flag 1)         | Web       | shuga__            | Web/botnet_1                  | botnet.ctf.hackucf.org              | 24301 | https://botnet.ctf.hackucf.org             |
| BotNet (flag 2)         | Web       | shuga__            | Web/botnet_2                  | botnet.ctf.hackucf.org              | 24301 | https://botnet.ctf.hackucf.org             |
| File Islands            | Web       | alimuhammadsecured | Web/File_Islands_1            | file-islands.ctf.hackucf.org        | 24303 | https://file-islands.ctf.hackucf.org       |
| Puzzling                | Web       | tsuto              | Web/Puzzling                  | puzzling.ctf.hackucf.org            | 24304 | https://puzzling.ctf.hackucf.org           |
| Doubly Secure           | Crypto    | Helithumper        | Crypto/doubly-secure          | doubly-secure.ctf.hackucf.org       | 24401 | https://doubly-secure.ctf.hackucf.org      |
| Mariner                 | Misc      | aleccoder          | Misc/mariner                  | ctf.hackucf.org                          | 24501 | `https://ctf.hackucf.org:24501` (Kubernetes API)|
| Melbourne               | i95       | guyinatuxedo       | i95/i95-melbourne             | ctf.hackucf.org                          | 24601 | `nc ctf.hackucf.org 24601`                      |
| Canaveral               | i95       | guyinatuxedo       | i95/i95-canaveral             | ctf.hackucf.org                          | 24602 | `nc ctf.hackucf.org 24602`                      |
| Palm Beach              | i95       | guyinatuxedo       | i95/i95-palmbeach             | ctf.hackucf.org                          | 24603 | `nc ctf.hackucf.org 24603`                      |
| Daytona Beach           | i95       | guyinatuxedo       | i95/i95-daytonabeach          | ctf.hackucf.org                          | 24605 | `nc ctf.hackucf.org 24605`                      |
| Fort Pierce             | i95       | guyinatuxedo       | i95/i95-fortpierce            | ctf.hackucf.org                          | 24606 | `nc ctf.hackucf.org 24606`                      |
| Titusville              | i95       | guyinatuxedo       | i95/i95-titusville            | ctf.hackucf.org                          | 24607 | `nc ctf.hackucf.org 24607`                      |
| Jacksonville            | i95       | guyinatuxedo       | i95/i95-jacksonville          | ctf.hackucf.org                          | 24608 | `nc ctf.hackucf.org 24608`                      |
| Jupiter                 | i95       | guyinatuxedo       | i95/i95-jupiter               | ctf.hackucf.org                          | 24609 | `nc ctf.hackucf.org 24609`                      |
| Boca Raton              | i95       | guyinatuxedo       | i95/i95-bocaraton             | ctf.hackucf.org                          | 24610 | `nc ctf.hackucf.org 24610`                      |

Notes:

* BotNet (`Web/botnet_1` + `Web/botnet_2`) is a single deployment that serves
  both flags from one container (port 24301).
* Port 24604 is intentionally unassigned (nine i95 challenges; no challenge was
  numbered 24604).
* Mariner is a k3s/Kubernetes API server reached over raw TLS on 24501, not an
  HTTP challenge, so it has no nginx subdomain.
