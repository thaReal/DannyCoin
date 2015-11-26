import ecdsa

genesis_key = ecdsa.SigningKey.generate(curve=ecdsa.SECP256k1)
pub_x = hex(genesis_key.privkey.public_key.point.x())
pub_y = hex(genesis_key.privkey.public_key.point.y())



#Test values
#pubx:
#e9050d0d22944a5be106481d8530cffe73b5e561d73badbe5af163c270b0c7ea
#puby:
#7d17abd7ac9aaae0d2c75f691fbe49b418226be9ce57fddd02dc32dc4b401919
#and publickey:
#04e9050d0d22944a5be106481d8530cffe73b5e561d73badbe5af163c270b0c7ea7d17abd7ac9aaae0d2c75f691fbe49b418226be9ce57fddd02dc32dc4b401919


