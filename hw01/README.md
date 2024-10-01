## Задание

Написать программу, которая вычисляет и печатает характеристики кэша данных L1:
- Объем
- Длина линейки
- Ассоциативность

## Решение

[main.cpp](main.cpp)

Результат запуска на apple m1:
```
L1 cache: 64 Kb, 8-ways, 128 bytes line
```

Результат запуска на intel core i5 u8250:
```
L1 cache: 32 Kb, 8-ways, 64 bytes line
```

<details>
<summary>Полный лог для m1 с VERBOSE = 1</summary>

```
Times for way size 16 bytes:
associativity = 1, time = 53317666 ns, diff = 100%
associativity = 2, time = 52805208 ns, diff = -0.970469%
associativity = 3, time = 53807167 ns, diff = 1.86213%
associativity = 4, time = 52871917 ns, diff = -1.7689%
associativity = 5, time = 53060625 ns, diff = 0.355646%
associativity = 6, time = 52675459 ns, diff = -0.731206%
associativity = 7, time = 53109000 ns, diff = 0.816323%
associativity = 8, time = 52707792 ns, diff = -0.761193%
associativity = 9, time = 52592250 ns, diff = -0.219694%
associativity = 10, time = 52763667 ns, diff = 0.324877%
associativity = 11, time = 52491791 ns, diff = -0.51794%
associativity = 12, time = 53423792 ns, diff = 1.74454%
associativity = 13, time = 53193000 ns, diff = -0.433877%
associativity = 14, time = 53102417 ns, diff = -0.170582%
associativity = 15, time = 53144459 ns, diff = 0.0791089%
associativity = 16, time = 53616542 ns, diff = 0.88048%

Times for way size 32 bytes:
associativity = 1, time = 52326583 ns, diff = 100%
associativity = 2, time = 53166292 ns, diff = 1.5794%
associativity = 3, time = 53011542 ns, diff = -0.291918%
associativity = 4, time = 54462625 ns, diff = 2.66436%
associativity = 5, time = 53875375 ns, diff = -1.09002%
associativity = 6, time = 54528542 ns, diff = 1.19784%
associativity = 7, time = 54415417 ns, diff = -0.207891%
associativity = 8, time = 59416625 ns, diff = 8.41719%
associativity = 9, time = 55114250 ns, diff = -7.80628%
associativity = 10, time = 54157000 ns, diff = -1.76755%
associativity = 11, time = 53672750 ns, diff = -0.902227%
associativity = 12, time = 54505125 ns, diff = 1.52715%
associativity = 13, time = 55816084 ns, diff = 2.34871%
associativity = 14, time = 53847667 ns, diff = -3.65553%
associativity = 15, time = 61177917 ns, diff = 11.9819%
associativity = 16, time = 67123292 ns, diff = 8.85739%

Times for way size 64 bytes:
associativity = 1, time = 59400334 ns, diff = 100%
associativity = 2, time = 54008750 ns, diff = -9.9828%
associativity = 3, time = 53420250 ns, diff = -1.10164%
associativity = 4, time = 56249500 ns, diff = 5.02982%
associativity = 5, time = 54004166 ns, diff = -4.15771%
associativity = 6, time = 59617417 ns, diff = 9.41545%
associativity = 7, time = 52513750 ns, diff = -13.5273%
associativity = 8, time = 56281833 ns, diff = 6.69503%
associativity = 9, time = 53146667 ns, diff = -5.89908%
associativity = 10, time = 52655875 ns, diff = -0.932075%
associativity = 11, time = 52184375 ns, diff = -0.903527%
associativity = 12, time = 52451084 ns, diff = 0.508491%
associativity = 13, time = 53400125 ns, diff = 1.77723%
associativity = 14, time = 52790375 ns, diff = -1.15504%
associativity = 15, time = 53800583 ns, diff = 1.87769%
associativity = 16, time = 53195791 ns, diff = -1.13692%

Times for way size 128 bytes:
associativity = 1, time = 53421542 ns, diff = 100%
associativity = 2, time = 53321791 ns, diff = -0.187074%
associativity = 3, time = 52810500 ns, diff = -0.968162%
associativity = 4, time = 52801666 ns, diff = -0.0167305%
associativity = 5, time = 52881542 ns, diff = 0.151047%
associativity = 6, time = 53409708 ns, diff = 0.988895%
associativity = 7, time = 53525291 ns, diff = 0.215941%
associativity = 8, time = 52870625 ns, diff = -1.23824%
associativity = 9, time = 52852375 ns, diff = -0.0345301%
associativity = 10, time = 53148917 ns, diff = 0.557946%
associativity = 11, time = 52458500 ns, diff = -1.31612%
associativity = 12, time = 51360375 ns, diff = -2.13808%
associativity = 13, time = 52872000 ns, diff = 2.85903%
associativity = 14, time = 52235458 ns, diff = -1.2186%
associativity = 15, time = 51835750 ns, diff = -0.771105%
associativity = 16, time = 51109166 ns, diff = -1.42163%

Times for way size 256 bytes:
associativity = 1, time = 52757417 ns, diff = 100%
associativity = 2, time = 50361334 ns, diff = -4.75778%
associativity = 3, time = 52300500 ns, diff = 3.70774%
associativity = 4, time = 52418084 ns, diff = 0.22432%
associativity = 5, time = 51116541 ns, diff = -2.54623%
associativity = 6, time = 52012042 ns, diff = 1.72172%
associativity = 7, time = 52564666 ns, diff = 1.05132%
associativity = 8, time = 51803666 ns, diff = -1.46901%
associativity = 9, time = 51321792 ns, diff = -0.938927%
associativity = 10, time = 51995500 ns, diff = 1.2957%
associativity = 11, time = 51401292 ns, diff = -1.15602%
associativity = 12, time = 50909083 ns, diff = -0.966839%
associativity = 13, time = 51465833 ns, diff = 1.08179%
associativity = 14, time = 50543875 ns, diff = -1.82407%
associativity = 15, time = 51856167 ns, diff = 2.53064%
associativity = 16, time = 53916375 ns, diff = 3.82112%

Times for way size 512 bytes:
associativity = 1, time = 53164333 ns, diff = 100%
associativity = 2, time = 52993209 ns, diff = -0.322917%
associativity = 3, time = 53050458 ns, diff = 0.107914%
associativity = 4, time = 53199916 ns, diff = 0.280937%
associativity = 5, time = 53325792 ns, diff = 0.236051%
associativity = 6, time = 52449333 ns, diff = -1.67106%
associativity = 7, time = 51282250 ns, diff = -2.2758%
associativity = 8, time = 51309042 ns, diff = 0.0522169%
associativity = 9, time = 51799875 ns, diff = 0.947556%
associativity = 10, time = 51386458 ns, diff = -0.804525%
associativity = 11, time = 51942333 ns, diff = 1.07018%
associativity = 12, time = 51101167 ns, diff = -1.64608%
associativity = 13, time = 50893875 ns, diff = -0.407302%
associativity = 14, time = 50829125 ns, diff = -0.127388%
associativity = 15, time = 51231834 ns, diff = 0.786052%
associativity = 16, time = 51522750 ns, diff = 0.564636%

Times for way size 1 Kb:
associativity = 1, time = 50964625 ns, diff = 100%
associativity = 2, time = 51555333 ns, diff = 1.14577%
associativity = 3, time = 50752167 ns, diff = -1.58253%
associativity = 4, time = 50475584 ns, diff = -0.547954%
associativity = 5, time = 51371000 ns, diff = 1.74304%
associativity = 6, time = 50978917 ns, diff = -0.769108%
associativity = 7, time = 51593500 ns, diff = 1.1912%
associativity = 8, time = 51027458 ns, diff = -1.10929%
associativity = 9, time = 51625708 ns, diff = 1.15882%
associativity = 10, time = 52910708 ns, diff = 2.42862%
associativity = 11, time = 51989792 ns, diff = -1.77134%
associativity = 12, time = 51435250 ns, diff = -1.07814%
associativity = 13, time = 50769458 ns, diff = -1.3114%
associativity = 14, time = 51634291 ns, diff = 1.67492%
associativity = 15, time = 52063000 ns, diff = 0.823443%
associativity = 16, time = 51732125 ns, diff = -0.639593%

Times for way size 2 Kb:
associativity = 1, time = 51802333 ns, diff = 100%
associativity = 2, time = 52247667 ns, diff = 0.852352%
associativity = 3, time = 51354250 ns, diff = -1.73971%
associativity = 4, time = 51186417 ns, diff = -0.327886%
associativity = 5, time = 51398541 ns, diff = 0.412704%
associativity = 6, time = 51869333 ns, diff = 0.90765%
associativity = 7, time = 52052750 ns, diff = 0.352368%
associativity = 8, time = 52865875 ns, diff = 1.53809%
associativity = 9, time = 50286541 ns, diff = -5.12927%
associativity = 10, time = 50594708 ns, diff = 0.609089%
associativity = 11, time = 51034791 ns, diff = 0.86232%
associativity = 12, time = 52037500 ns, diff = 1.9269%
associativity = 13, time = 51369000 ns, diff = -1.30137%
associativity = 14, time = 51631291 ns, diff = 0.508008%
associativity = 15, time = 50809417 ns, diff = -1.61756%
associativity = 16, time = 51321166 ns, diff = 0.99715%

Times for way size 4 Kb:
associativity = 1, time = 51551958 ns, diff = 100%
associativity = 2, time = 51874708 ns, diff = 0.622172%
associativity = 3, time = 52019917 ns, diff = 0.279141%
associativity = 4, time = 51539250 ns, diff = -0.932623%
associativity = 5, time = 50626500 ns, diff = -1.80291%
associativity = 6, time = 51515042 ns, diff = 1.72482%
associativity = 7, time = 51435625 ns, diff = -0.154401%
associativity = 8, time = 50826542 ns, diff = -1.19836%
associativity = 9, time = 50696166 ns, diff = -0.257171%
associativity = 10, time = 51047458 ns, diff = 0.688167%
associativity = 11, time = 50332541 ns, diff = -1.42039%
associativity = 12, time = 50903083 ns, diff = 1.12084%
associativity = 13, time = 50512584 ns, diff = -0.773073%
associativity = 14, time = 51046958 ns, diff = 1.04683%
associativity = 15, time = 56087500 ns, diff = 8.98693%
associativity = 16, time = 51753417 ns, diff = -8.37449%

Times for way size 8 Kb:
associativity = 1, time = 51510291 ns, diff = 100%
associativity = 2, time = 50795084 ns, diff = -1.40802%
associativity = 3, time = 50780458 ns, diff = -0.0288024%
associativity = 4, time = 51940709 ns, diff = 2.2338%
associativity = 5, time = 52267041 ns, diff = 0.624355%
associativity = 6, time = 52254209 ns, diff = -0.0245569%
associativity = 7, time = 50924292 ns, diff = -2.61156%
associativity = 8, time = 51788583 ns, diff = 1.66888%
associativity = 9, time = 51731000 ns, diff = -0.111312%
associativity = 10, time = 51583625 ns, diff = -0.285701%
associativity = 11, time = 50583833 ns, diff = -1.97651%
associativity = 12, time = 51373709 ns, diff = 1.53751%
associativity = 13, time = 50528250 ns, diff = -1.67324%
associativity = 14, time = 49931708 ns, diff = -1.19472%
associativity = 15, time = 51442167 ns, diff = 2.93623%
associativity = 16, time = 51518542 ns, diff = 0.148248%

Times for way size 16 Kb:
associativity = 1, time = 51350375 ns, diff = 100%
associativity = 2, time = 51171875 ns, diff = -0.348824%
associativity = 3, time = 50090125 ns, diff = -2.15961%
associativity = 4, time = 50902416 ns, diff = 1.59578%
associativity = 5, time = 51580000 ns, diff = 1.31366%
associativity = 6, time = 50820709 ns, diff = -1.49406%
associativity = 7, time = 51086500 ns, diff = 0.520276%
associativity = 8, time = 52645667 ns, diff = 2.96162%
associativity = 9, time = 273528291 ns, diff = 80.7531%
associativity = 10, time = 272756666 ns, diff = -0.282899%
associativity = 11, time = 285103542 ns, diff = 4.33066%
associativity = 12, time = 265289458 ns, diff = -7.46885%
associativity = 13, time = 256405625 ns, diff = -3.46476%
associativity = 14, time = 276266000 ns, diff = 7.18886%
associativity = 15, time = 256752167 ns, diff = -7.60026%
associativity = 16, time = 266579000 ns, diff = 3.68627%

Times for way size 32 Kb:
associativity = 1, time = 50468917 ns, diff = 100%
associativity = 2, time = 51007333 ns, diff = 1.05557%
associativity = 3, time = 50801042 ns, diff = -0.406076%
associativity = 4, time = 50619750 ns, diff = -0.358145%
associativity = 5, time = 50959792 ns, diff = 0.667275%
associativity = 6, time = 50179250 ns, diff = -1.55551%
associativity = 7, time = 50895333 ns, diff = 1.40697%
associativity = 8, time = 50831625 ns, diff = -0.125331%
associativity = 9, time = 253753750 ns, diff = 79.9681%
associativity = 10, time = 260037208 ns, diff = 2.41637%
associativity = 11, time = 251177417 ns, diff = -3.5273%
associativity = 12, time = 279269667 ns, diff = 10.0592%
associativity = 13, time = 262878916 ns, diff = -6.2351%
associativity = 14, time = 270381792 ns, diff = 2.77492%
associativity = 15, time = 252891958 ns, diff = -6.91593%
associativity = 16, time = 251949208 ns, diff = -0.374183%

Times for way size 64 Kb:
associativity = 1, time = 50808958 ns, diff = 100%
associativity = 2, time = 50572000 ns, diff = -0.468556%
associativity = 3, time = 50703000 ns, diff = 0.258367%
associativity = 4, time = 50284166 ns, diff = -0.832934%
associativity = 5, time = 51870291 ns, diff = 3.05787%
associativity = 6, time = 51708875 ns, diff = -0.312163%
associativity = 7, time = 50519375 ns, diff = -2.35454%
associativity = 8, time = 51336625 ns, diff = 1.59194%
associativity = 9, time = 265693958 ns, diff = 80.6783%
associativity = 10, time = 257315584 ns, diff = -3.25607%
associativity = 11, time = 272232375 ns, diff = 5.47943%
associativity = 12, time = 256794459 ns, diff = -6.01178%
associativity = 13, time = 279374958 ns, diff = 8.08251%
associativity = 14, time = 288108750 ns, diff = 3.03142%
associativity = 15, time = 258557333 ns, diff = -11.4293%
associativity = 16, time = 280625125 ns, diff = 7.8638%

Times for way size 128 Kb:
associativity = 1, time = 50624750 ns, diff = 100%
associativity = 2, time = 50877333 ns, diff = 0.496455%
associativity = 3, time = 50675084 ns, diff = -0.399109%
associativity = 4, time = 60697041 ns, diff = 16.5114%
associativity = 5, time = 51896375 ns, diff = -16.9582%
associativity = 6, time = 50670417 ns, diff = -2.41947%
associativity = 7, time = 51416125 ns, diff = 1.45034%
associativity = 8, time = 51157625 ns, diff = -0.505301%
associativity = 9, time = 280601666 ns, diff = 81.7686%
associativity = 10, time = 286274083 ns, diff = 1.98146%
associativity = 11, time = 259253875 ns, diff = -10.4223%
associativity = 12, time = 253121292 ns, diff = -2.42278%
associativity = 13, time = 266371708 ns, diff = 4.97441%
associativity = 14, time = 279499541 ns, diff = 4.69691%
associativity = 15, time = 271225667 ns, diff = -3.05055%
associativity = 16, time = 283205959 ns, diff = 4.23024%

Times for way size 256 Kb:
associativity = 1, time = 50470458 ns, diff = 100%
associativity = 2, time = 51675209 ns, diff = 2.33139%
associativity = 3, time = 51069792 ns, diff = -1.18547%
associativity = 4, time = 51581459 ns, diff = 0.991959%
associativity = 5, time = 51585958 ns, diff = 0.00872137%
associativity = 6, time = 50803959 ns, diff = -1.53925%
associativity = 7, time = 50493625 ns, diff = -0.6146%
associativity = 8, time = 51193333 ns, diff = 1.3668%
associativity = 9, time = 271877500 ns, diff = 81.1704%
associativity = 10, time = 269348083 ns, diff = -0.939089%
associativity = 11, time = 294934500 ns, diff = 8.67529%
associativity = 12, time = 296564584 ns, diff = 0.549656%
associativity = 13, time = 347842292 ns, diff = 14.7417%
associativity = 14, time = 375757333 ns, diff = 7.42901%
associativity = 15, time = 365228541 ns, diff = -2.8828%
associativity = 16, time = 379842375 ns, diff = 3.84734%

Times for way size 512 Kb:
associativity = 1, time = 51301125 ns, diff = 100%
associativity = 2, time = 51606292 ns, diff = 0.591337%
associativity = 3, time = 51067792 ns, diff = -1.05448%
associativity = 4, time = 51274500 ns, diff = 0.40314%
associativity = 5, time = 51838875 ns, diff = 1.08871%
associativity = 6, time = 82188375 ns, diff = 36.9268%
associativity = 7, time = 105704125 ns, diff = 22.2468%
associativity = 8, time = 145198459 ns, diff = 27.2002%
associativity = 9, time = 382119875 ns, diff = 62.0019%
associativity = 10, time = 368772250 ns, diff = -3.61948%
associativity = 11, time = 359807583 ns, diff = -2.49152%
associativity = 12, time = 365050042 ns, diff = 1.43609%
associativity = 13, time = 370094916 ns, diff = 1.36313%
associativity = 14, time = 351322500 ns, diff = -5.34336%
associativity = 15, time = 388842000 ns, diff = 9.64903%
associativity = 16, time = 372279375 ns, diff = -4.44898%

Times for way size 1 Mb:
associativity = 1, time = 50719709 ns, diff = 100%
associativity = 2, time = 51483125 ns, diff = 1.48285%
associativity = 3, time = 50906417 ns, diff = -1.13288%
associativity = 4, time = 50842125 ns, diff = -0.126454%
associativity = 5, time = 51468750 ns, diff = 1.21749%
associativity = 6, time = 82954250 ns, diff = 37.9553%
associativity = 7, time = 106698417 ns, diff = 22.2535%
associativity = 8, time = 148164459 ns, diff = 27.9865%
associativity = 9, time = 386383833 ns, diff = 61.6536%
associativity = 10, time = 363839834 ns, diff = -6.19613%
associativity = 11, time = 364838458 ns, diff = 0.273717%
associativity = 12, time = 379406916 ns, diff = 3.8398%
associativity = 13, time = 364410791 ns, diff = -4.11517%
associativity = 14, time = 367191958 ns, diff = 0.757415%
associativity = 15, time = 367383584 ns, diff = 0.0521597%
associativity = 16, time = 365916875 ns, diff = -0.400831%

Times for way size 2 Mb:
associativity = 1, time = 50571083 ns, diff = 100%
associativity = 2, time = 50243667 ns, diff = -0.651656%
associativity = 3, time = 50503959 ns, diff = 0.515389%
associativity = 4, time = 51818250 ns, diff = 2.53635%
associativity = 5, time = 52668958 ns, diff = 1.6152%
associativity = 6, time = 83934333 ns, diff = 37.2498%
associativity = 7, time = 107239458 ns, diff = 21.7319%
associativity = 8, time = 150993833 ns, diff = 28.9776%
associativity = 9, time = 377224458 ns, diff = 59.9724%
associativity = 10, time = 355211875 ns, diff = -6.19703%
associativity = 11, time = 362352542 ns, diff = 1.97064%
associativity = 12, time = 384218750 ns, diff = 5.69108%
associativity = 13, time = 362900167 ns, diff = -5.8745%
associativity = 14, time = 363582167 ns, diff = 0.187578%
associativity = 15, time = 380813584 ns, diff = 4.5249%
associativity = 16, time = 368939292 ns, diff = -3.21849%

Times for way size 4 Mb:
associativity = 1, time = 50863292 ns, diff = 100%
associativity = 2, time = 50252083 ns, diff = -1.21629%
associativity = 3, time = 50346958 ns, diff = 0.188442%
associativity = 4, time = 50765708 ns, diff = 0.824868%
associativity = 5, time = 51521416 ns, diff = 1.46678%
associativity = 6, time = 82805833 ns, diff = 37.7805%
associativity = 7, time = 108748583 ns, diff = 23.8557%
associativity = 8, time = 147046416 ns, diff = 26.0447%
associativity = 9, time = 370908583 ns, diff = 60.3551%
associativity = 10, time = 371496541 ns, diff = 0.158267%
associativity = 11, time = 347037333 ns, diff = -7.048%
associativity = 12, time = 381022875 ns, diff = 8.91955%
associativity = 13, time = 835144416 ns, diff = 54.3764%
associativity = 14, time = 803138042 ns, diff = -3.98516%
associativity = 15, time = 823390667 ns, diff = 2.45966%
associativity = 16, time = 827442958 ns, diff = 0.489737%

Times for way size 8 Mb:
associativity = 1, time = 52883458 ns, diff = 100%
associativity = 2, time = 53125125 ns, diff = 0.454902%
associativity = 3, time = 53391125 ns, diff = 0.49821%
associativity = 4, time = 53456125 ns, diff = 0.121595%
associativity = 5, time = 54629875 ns, diff = 2.14855%
associativity = 6, time = 100177833 ns, diff = 45.4671%
associativity = 7, time = 151253792 ns, diff = 33.7684%
associativity = 8, time = 154660584 ns, diff = 2.20275%
associativity = 9, time = 389544791 ns, diff = 60.2971%
associativity = 10, time = 364702334 ns, diff = -6.81171%
associativity = 11, time = 357473166 ns, diff = -2.0223%
associativity = 12, time = 360671708 ns, diff = 0.886829%
associativity = 13, time = 837877459 ns, diff = 56.9541%
associativity = 14, time = 826940958 ns, diff = -1.32253%
associativity = 15, time = 828167125 ns, diff = 0.148058%
associativity = 16, time = 826702458 ns, diff = -0.17717%

Times for way size 16 Mb:
associativity = 1, time = 55886041 ns, diff = 100%
associativity = 2, time = 53439083 ns, diff = -4.57897%
associativity = 3, time = 52104750 ns, diff = -2.56087%
associativity = 4, time = 51623708 ns, diff = -0.931824%
associativity = 5, time = 52456458 ns, diff = 1.58751%
associativity = 6, time = 83808500 ns, diff = 37.4091%
associativity = 7, time = 105418833 ns, diff = 20.4995%
associativity = 8, time = 147539208 ns, diff = 28.5486%
associativity = 9, time = 370882750 ns, diff = 60.2194%
associativity = 10, time = 363613791 ns, diff = -1.99909%
associativity = 11, time = 356694208 ns, diff = -1.93992%
associativity = 12, time = 372289333 ns, diff = 4.18898%
associativity = 13, time = 853399958 ns, diff = 56.3757%
associativity = 14, time = 832257959 ns, diff = -2.54032%
associativity = 15, time = 844530125 ns, diff = 1.45314%
associativity = 16, time = 817960792 ns, diff = -3.24824%

Times for way size 32 Mb:
associativity = 1, time = 50910250 ns, diff = 100%
associativity = 2, time = 51262500 ns, diff = 0.687149%
associativity = 3, time = 50909209 ns, diff = -0.693963%
associativity = 4, time = 51268250 ns, diff = 0.700318%
associativity = 5, time = 51475500 ns, diff = 0.402619%
associativity = 6, time = 82768542 ns, diff = 37.8079%
associativity = 7, time = 108414375 ns, diff = 23.6554%
associativity = 8, time = 146191250 ns, diff = 25.8407%
associativity = 9, time = 369083750 ns, diff = 60.3908%
associativity = 10, time = 357645208 ns, diff = -3.19829%
associativity = 11, time = 370708166 ns, diff = 3.52378%
associativity = 12, time = 364545458 ns, diff = -1.69052%
associativity = 13, time = 830905083 ns, diff = 56.1267%
associativity = 14, time = 822745125 ns, diff = -0.991797%
associativity = 15, time = 805030209 ns, diff = -2.20053%
associativity = 16, time = 825393833 ns, diff = 2.46714%

Jumps for way: 
  - 16 bytes: 
  - 32 bytes: 14 
  - 64 bytes: 
  - 128 bytes: 
  - 256 bytes: 
  - 512 bytes: 
  - 1 Kb: 
  - 2 Kb: 
  - 4 Kb: 
  - 8 Kb: 
  - 16 Kb: 8 
  - 32 Kb: 8 11 
  - 64 Kb: 8 
  - 128 Kb: 3 8 
  - 256 Kb: 8 12 
  - 512 Kb: 5 6 7 8 
  - 1 Mb: 5 6 7 8 
  - 2 Mb: 5 6 7 8 
  - 4 Mb: 5 6 7 8 12 
  - 8 Mb: 5 6 8 12 
  - 16 Mb: 5 6 7 8 12 
  - 32 Mb: 5 6 7 8 12 

Possible cache params (cache_size, associativity):
  - (56 Mb, 7)
  - (24 Mb, 12)
  - (1 Mb, 5)
  - (1 Mb, 6)
  - (64 Kb, 8)

Times for cache line 16 bytes:
spots = 1, time = 50951167 ns, diff = 100%
spots = 2, time = 50541500 ns, diff = -0.810556%
spots = 4, time = 51521000 ns, diff = 1.90117%
spots = 8, time = 51479167 ns, diff = -0.081262%
spots = 16, time = 51977125 ns, diff = 0.958033%
spots = 32, time = 51474625 ns, diff = -0.976209%
spots = 64, time = 146834209 ns, diff = 64.9437%
spots = 128, time = 145954750 ns, diff = -0.602556%
spots = 256, time = 164340333 ns, diff = 11.1875%
spots = 512, time = 150416458 ns, diff = -9.25688%
spots = 1024, time = 598947917 ns, diff = 74.8866%

Times for cache line 32 bytes:
spots = 1, time = 51413000 ns, diff = -1064.97%
spots = 2, time = 51642375 ns, diff = 0.44416%
spots = 4, time = 50540959 ns, diff = -2.17925%
spots = 8, time = 50888791 ns, diff = 0.683514%
spots = 16, time = 51845084 ns, diff = 1.84452%
spots = 32, time = 50956042 ns, diff = -1.74472%
spots = 64, time = 146550375 ns, diff = 65.2297%
spots = 128, time = 148753541 ns, diff = 1.48108%
spots = 256, time = 147486542 ns, diff = -0.859061%
spots = 512, time = 147438500 ns, diff = -0.0325844%
spots = 1024, time = 146226084 ns, diff = -0.829138%

Times for cache line 64 bytes:
spots = 1, time = 50408167 ns, diff = -190.084%
spots = 2, time = 51299625 ns, diff = 1.73775%
spots = 4, time = 51280250 ns, diff = -0.0377826%
spots = 8, time = 51124917 ns, diff = -0.30383%
spots = 16, time = 51679167 ns, diff = 1.07248%
spots = 32, time = 51092666 ns, diff = -1.14792%
spots = 64, time = 145638792 ns, diff = 64.9182%
spots = 128, time = 146937250 ns, diff = 0.883682%
spots = 256, time = 145547042 ns, diff = -0.955161%
spots = 512, time = 146350375 ns, diff = 0.548911%
spots = 1024, time = 146639875 ns, diff = 0.197422%

Times for cache line 128 bytes:
spots = 1, time = 51084334 ns, diff = -187.054%
spots = 2, time = 51900625 ns, diff = 1.5728%
spots = 4, time = 52034916 ns, diff = 0.258079%
spots = 8, time = 51009375 ns, diff = -2.0105%
spots = 16, time = 51567750 ns, diff = 1.0828%
spots = 32, time = 49978250 ns, diff = -3.18038%
spots = 64, time = 148109959 ns, diff = 66.256%
spots = 128, time = 144742792 ns, diff = -2.32631%
spots = 256, time = 145338500 ns, diff = 0.409876%
spots = 512, time = 148221500 ns, diff = 1.94506%
spots = 1024, time = 145338667 ns, diff = -1.98353%

Times for cache line 256 bytes:
spots = 1, time = 51144584 ns, diff = -184.172%
spots = 2, time = 51318333 ns, diff = 0.338571%
spots = 4, time = 51310042 ns, diff = -0.0161586%
spots = 8, time = 52295584 ns, diff = 1.88456%
spots = 16, time = 51041667 ns, diff = -2.45665%
spots = 32, time = 50126541 ns, diff = -1.82563%
spots = 64, time = 146298416 ns, diff = 65.7368%
spots = 128, time = 242326125 ns, diff = 39.6275%
spots = 256, time = 246945917 ns, diff = 1.87077%
spots = 512, time = 271840125 ns, diff = 9.15767%
spots = 1024, time = 466945875 ns, diff = 41.7834%

Jumps for cache line
  - 16 bytes: 64 256 1024 
  - 32 bytes: 64 
  - 64 bytes: 64 
  - 128 bytes: 64 
  - 256 bytes: 64 128 1024 

L1 cache: 64 Kb, 8-ways, 128 bytes line
```
</details>
