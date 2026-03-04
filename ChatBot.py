import re
import unicodedata

def remove_accents(text):
    return ''.join(
        c for c in unicodedata.normalize('NFD', text)
        if unicodedata.category(c) != 'Mn'
    )

def clean_text(text):
    text = remove_accents(text)
    text = re.sub(r'[^\w\s]', '', text) 
    return text.lower()

patterns = {
    r'γεια|καλησπερα|καλημερα': "Γεια σου! Πώς μπορώ να σε βοηθήσω;",
    r'πως εισαι|τι κανεις': "είμαι καλά, εσυ πως εισαι ;",
    r'πως σε λενε': "Με λένε Chatbot!",
    r'ευχαριστω|thanks': "Παρακαλώ! 😊",
    r'ποια ομαδα υποστηριζεις|ποια ειναι η αγαπημενη σου ομαδα': "Γιατί σε ενδιαφέρει ποια ομάδα υποστηρίζω;",
    r'ποια ειναι η καλυτερη ομαδα': "Τι σε κάνει να πιστεύεις ότι υπάρχει μια 'καλύτερη' ομάδα;",
    r'ποιος ειναι ο καλυτερος παικτης': "Ποιος είναι για σένα ο καλύτερος παίκτης; Τι τον κάνει τόσο ξεχωριστό;",
    r'παρακολουθεις ποδοσφαιρο':  "Εσύ παρακολουθείς ποδόσφαιρο συχνά; Τι σε τραβάει σε αυτό;",  
    r'ποιος ειναι ο προπονητης της (.+)':"Θα ήθελες να μάθεις περισσότερα για τον προπονητή της {}; Τι σε ενδιαφέρει περισσότερο γι’ αυτόν;",  
    r'τι ωρα παιζει η (.+)': "Γιατί είναι σημαντικό για σένα να ξέρεις τι ώρα παίζει η {};",  
}

def chatbot_response(user_input):
    user_input = clean_text(user_input)  
    for pattern, response in patterns.items():
        match = re.search(pattern, user_input, re.IGNORECASE)
        if match:
            if "{" in response: 
                return response.format(match.group(1))
            return response
    return "Συγγνώμη, δεν καταλαβαίνω. Μπορείς να το πεις αλλιώς;"

print("Chatbot: Γεια! Γράψε κάτι για να ξεκινήσουμε.")
while True:
    user_input = input("Εσύ: ")
    if user_input.lower() == "εξοδος":
        print("Chatbot: Αντίο! 😊")
        break
    response = chatbot_response(user_input)
    print(f"Chatbot: {response}")
