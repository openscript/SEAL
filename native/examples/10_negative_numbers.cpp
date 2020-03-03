#include "examples.h"

using namespace std;
using namespace seal;

void negative_numbers() {
    print_example_banner("Negative numbers");

    EncryptionParameters parms(scheme_type::BFV);

    size_t poly_modulus_degree = 2048;
    parms.set_poly_modulus_degree(poly_modulus_degree);

    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));

    parms.set_plain_modulus(2097152);

    auto context = SEALContext::Create(parms);

    print_line(__LINE__);
    cout << "Set encryption parameters and print" << endl;
    print_parameters(context);
    cout << endl;

    KeyGenerator keygen(context);
    PublicKey public_key = keygen.public_key();
    SecretKey secret_key = keygen.secret_key();

    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);

    int initial = 1;

    Plaintext initial_plain(to_string(initial));
    Ciphertext current_encrypted;
    encryptor.encrypt(initial_plain, current_encrypted);
    cout << "Initialized with 0x" + initial_plain.to_string() << endl;
    cout << "Initial noise budget:" << decryptor.invariant_noise_budget(current_encrypted) << " bits" << endl;

    Plaintext decrypted_value;

    Plaintext sub_value(to_string(3));

    evaluator.sub_plain_inplace(current_encrypted, sub_value);
    cout << "Noise budget:" << decryptor.invariant_noise_budget(current_encrypted) << " bits" << endl;
    decryptor.decrypt(current_encrypted, decrypted_value);
    cout << "Result: 0x" + decrypted_value.to_string() << endl;
}
