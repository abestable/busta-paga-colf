#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>


using namespace std;

// Dizionario delle retribuzioni minime orarie per il livello B dal 2020 al 2025
map<int, double> retribuzioni_minime = {
    {2020, 6.03}, {2021, 6.10}, {2022, 6.21}, {2023, 6.31}, {2024, 6.62}, {2025, 6.68}
};

// Dizionari per ritenuta e contributo INPS dal 2020 al 2025
map<int, double> ritenute = {
    {2020, 0.40}, {2021, 0.40}, {2022, 0.41}, {2023, 0.41}, {2024, 0.42}, {2025, 0.42}
};

map<int, double> contributi_inps = {
    {2020, 1.60}, {2021, 1.62}, {2022, 1.64}, {2023, 1.65}, {2024, 1.66}, {2025, 1.68}
};

vector<string> festivita = {"01/01", "06/01", "25/04", "01/05", "02/06", "15/08", "01/11", "08/12", "25/12", "26/12"};
string patrono = "11/11";

map<string, int> giorni_lavorativi = {
    {"Tuesday", 3}, {"Friday", 3}
};

int calcola_mesi_assunzione(string data_assunzione, string mese_anno) {
    struct tm data_ass = {};
    struct tm data_ref = {};
    
    strptime(data_assunzione.c_str(), "%d/%m/%Y", &data_ass);
    strptime(("01/" + mese_anno).c_str(), "%d/%m/%Y", &data_ref);
    
    int mesi_di_assunzione = (data_ref.tm_year - data_ass.tm_year) * 12 + (data_ref.tm_mon - data_ass.tm_mon);
    return mesi_di_assunzione;
}

double calcola_retribuzione_oraria(string data_assunzione, string mese_anno) {
    int mesi_di_assunzione = calcola_mesi_assunzione(data_assunzione, mese_anno);
    int anno_riferimento = stoi(mese_anno.substr(3, 4));
    double retribuzione_base = retribuzioni_minime[anno_riferimento];
    
    double scatti_anzianita = 0.0;
    for (int i = 1; i <= (mesi_di_assunzione / 24); ++i) {
        int anno_scatto = stoi(data_assunzione.substr(6, 4)) + (i * 2);
        if (retribuzioni_minime.find(anno_scatto) != retribuzioni_minime.end()) {
            scatti_anzianita += retribuzioni_minime[anno_scatto] * 0.04;
        }
    }
    
    return retribuzione_base + scatti_anzianita;
}


// Mappa di conversione per i giorni della settimana in italiano
map<string, string> giorni_italiano = {
    {"Monday", "Lunedì"}, {"Tuesday", "Martedì"}, {"Wednesday", "Mercoledì"},
    {"Thursday", "Giovedì"}, {"Friday", "Venerdì"}, {"Saturday", "Sabato"},
    {"Sunday", "Domenica"}
};

int calcola_ore_lavorate(string mese_anno) {
    int mese = stoi(mese_anno.substr(0, 2));
    int anno = stoi(mese_anno.substr(3, 4));

    // Determina il numero di giorni nel mese
    int giorni_nel_mese;
    if (mese == 2) {
        bool is_leap = (anno % 4 == 0 && (anno % 100 != 0 || anno % 400 == 0));
        giorni_nel_mese = is_leap ? 29 : 28;
    } else if (mese == 4 || mese == 6 || mese == 9 || mese == 11) {
        giorni_nel_mese = 30;
    } else {
        giorni_nel_mese = 31;
    }

    int ore_totali = 0;
    int ore_festivita = 0;

    cout << "\n--- Dettaglio Giorni Lavorati ---" << endl;

    for (int giorno = 1; giorno <= giorni_nel_mese; ++giorno) {
        struct tm data_corrente = {0};
        data_corrente.tm_year = anno - 1900;
        data_corrente.tm_mon = mese - 1;
        data_corrente.tm_mday = giorno;
        mktime(&data_corrente);

        char buffer[10];
        strftime(buffer, sizeof(buffer), "%d/%m", &data_corrente);
        string data_formattata = buffer;

        char giorno_settimana[10];
        strftime(giorno_settimana, sizeof(giorno_settimana), "%A", &data_corrente);
        string giorno_str = giorno_settimana;

        // Traduzione in italiano
        string giorno_italiano = giorni_italiano[giorno_str];

        int ore_giornaliere = 0;
        bool festivo = (find(festivita.begin(), festivita.end(), data_formattata) != festivita.end()) || (data_formattata == patrono);

        if (festivo) {
            if (giorni_lavorativi.find(giorno_str) != giorni_lavorativi.end()) {
                ore_giornaliere = giorni_lavorativi[giorno_str]; // Se è lavorativo, paga le ore normali
            } else {
                ore_giornaliere = 1; // Se è solo festivo e non lavorativo, paga 1 ora extra
            }
            ore_festivita += 1;
        } else if (giorni_lavorativi.find(giorno_str) != giorni_lavorativi.end()) {
            ore_giornaliere = giorni_lavorativi[giorno_str];
        }

        ore_totali += ore_giornaliere;

        // **Stampa solo se le ore lavorate sono > 0**
        if (ore_giornaliere > 0) {
            cout << data_formattata << " (" << giorno_italiano << "): " << ore_giornaliere << " ore"
                 << (festivo ? (giorni_lavorativi.find(giorno_str) != giorni_lavorativi.end() ? " (Festivo e non lavorato)" : " (Festivo non lavorativo - pagato 1 ora)") : "") << endl;
        }
    }

    cout << "Totale ore lavorate nel mese: " << ore_totali << endl;
    //    cout << "Ore per festività aggiunte: " << ore_festivita << endl;

    return ore_totali;
}

void genera_busta_paga(const std::string& data_assunzione, const std::string& mese_anno, const double& ferie_residue,  const double& ferie_godute, const std::string& lavoratore,  const std::string& datore) {

    cout << fixed << setprecision(2);
    cout << "\n--- Busta Paga ---" << endl;
    cout << "Datore di lavoro : " << datore << endl; 
    cout << "Lavoratore: " << lavoratore << endl; 
    double retribuzione_oraria = calcola_retribuzione_oraria(data_assunzione, mese_anno);
    int anno_riferimento = stoi(mese_anno.substr(3, 4));
    double ritenuta = ritenute[anno_riferimento];
    double contributo_inps = contributi_inps[anno_riferimento];
    
    int ore_lavorate = calcola_ore_lavorate(mese_anno);
    double totale_retribuzione = ore_lavorate * retribuzione_oraria;
    double quota_tredicesima = (6 * 4.33 * retribuzione_oraria) / 12;
    double tfr_mese = (totale_retribuzione + quota_tredicesima) / 13.5;
    double totale_lordo = totale_retribuzione + quota_tredicesima + tfr_mese;
    double ritenute_totali = ore_lavorate * ritenuta;
    double retribuzione_netto = totale_lordo - ritenute_totali;
    double ferie_maturate = 2.1666666666;
    double totale_contributi = ore_lavorate*(contributo_inps) +  ore_lavorate*(0.08);
    int mesi_di_assunzione = calcola_mesi_assunzione(data_assunzione, mese_anno);
    
    cout << "\n--- Retribuzione, TFR, Tredicesima  ---" << endl;
    cout << "Mesi di anzianità: " << mesi_di_assunzione << endl;
    cout << "Anno di riferimento: " << anno_riferimento << endl;
    cout << "Mese della busta paga : " << mese_anno << endl;
    cout << "Ritenuta (" << anno_riferimento << ") : " << ritenuta << " €" << endl;
    cout << "Ore Retribuite: " << ore_lavorate << endl;
    cout << "Retribuzione Oraria: " << retribuzione_oraria << " €" << endl;
    cout << "Ore * Retribuzione Oraria: " << retribuzione_oraria*ore_lavorate << " €" << endl;
    cout << "Tredicesima: " << quota_tredicesima << " €" << endl;
    cout << "Totale: " << quota_tredicesima+retribuzione_oraria*ore_lavorate << " €" << endl;
    cout << "TFR: " << tfr_mese << " €" << endl;
    cout << "Totale Lordo (incluso TFR): " << totale_lordo << " €" << endl;
    cout << "Ritenute: " << ritenuta*ore_lavorate << " €" << endl;
    cout << "Retribuzione Netta: " << retribuzione_netto << " €" << endl;


    cout << "\n--- Ferie ---" << endl;
    cout << "Ferie residue dal mese precedente: " << ferie_residue << " ore" << endl;
    cout << "Ferie godute: " << ferie_godute << " ore" << endl;
    cout << "Ferie maturate in un mese: " << ferie_maturate << " ore" << endl;
    cout << "Ferie residue: " << ferie_maturate+ferie_residue-ferie_godute << " ore" << endl;



    cout << "\n--- Contribuzione ---" << endl;
    cout << "Contributo INPS (" << anno_riferimento << ") : "  << contributo_inps << " €" << endl;
    cout << "Ritenuta (" << anno_riferimento << ") : " << ritenuta << " €" << endl;
    cout << "Contribuiti a carico del datore di lavoro: " << ore_lavorate*(contributo_inps-ritenuta) << " €" << endl;
    cout << "Contribuiti a carico del lavoratore: " << ore_lavorate*(ritenuta) << " €" << endl;
    cout << "Cassa colf a carico del datore di lavoro: " << ore_lavorate*(0.02) << " €" << endl;
    cout << "Cassa colf a carico del lavoratore: " << ore_lavorate*(0.06) << " €" << endl;
    cout << "Totale contributi e cassa colf versati al INPS tutti a carico dal datore: " << totale_contributi << " €" << endl;
    
}

int main(int argc, char* argv[]) {
    if (argc != 9) {
        std::cerr << "Errore: devi fornire in quest'ordine: data di assunzione (GG/MM/AAAA); mese e l'anno della busta (MM/AAAA); ferie residue in ore; ferie godute in ore; nome e cognome del lavoratore;nome e cognome del datore." << std::endl;
        return 1;
    }

    std::string data_assunzione = argv[1];
    std::string mese_anno = argv[2];
    std::string ferie_residue_str = argv[3];
    std::string ferie_godute_str = argv[4];
    std::string lavoratore_nome = argv[5];
    std::string lavoratore_cognome = argv[6];
    std::string datore_nome = argv[7];
    std::string datore_cognome = argv[8];
    double ferie_residue = std::stod(ferie_residue_str);
    double ferie_godute  = std::stod(ferie_godute_str);

    std::string lavoratore;
    lavoratore  = lavoratore_nome + " " + lavoratore_cognome;

    std::string datore;
    datore  = datore_nome + " " + datore_cognome;

    genera_busta_paga(data_assunzione, mese_anno, ferie_residue, ferie_godute, lavoratore, datore);
    return 0;
}
