const INITIAL_BOOKS = [
  { id: 1, title: "Introduction to Computing Systems", author: "Yale N. Patt", available: true, borrower: "", waitlist: [] },
  { id: 2, title: "Let Us C", author: "Yashavant Kanetkar", available: true, borrower: "", waitlist: [] },
  { id: 3, title: "C Programming Language", author: "Brian W. Kernighan", available: true, borrower: "", waitlist: [] },
  { id: 4, title: "Object-Oriented Programming in C++", author: "Robert Lafore", available: true, borrower: "", waitlist: [] },
  { id: 5, title: "Data Structures and Algorithms", author: "Mark Allen Weiss", available: true, borrower: "", waitlist: [] },
  { id: 6, title: "Discrete Mathematics and Its Applications", author: "Kenneth H. Rosen", available: true, borrower: "", waitlist: [] },
  { id: 7, title: "Database System Concepts", author: "Abraham Silberschatz", available: true, borrower: "", waitlist: [] },
  { id: 8, title: "Operating System Concepts", author: "Abraham Silberschatz", available: true, borrower: "", waitlist: [] },
  { id: 9, title: "Computer Networking: A Top-Down Approach", author: "James Kurose", available: true, borrower: "", waitlist: [] },
  { id: 10, title: "Computer Organization and Design", author: "David A. Patterson", available: true, borrower: "", waitlist: [] },
  { id: 11, title: "Software Engineering", author: "Ian Sommerville", available: true, borrower: "", waitlist: [] },
  { id: 12, title: "Artificial Intelligence: A Modern Approach", author: "Stuart Russell", available: true, borrower: "", waitlist: [] },
  { id: 13, title: "Introduction to Algorithms", author: "Thomas H. Cormen", available: true, borrower: "", waitlist: [] },
  { id: 14, title: "Clean Code", author: "Robert C. Martin", available: true, borrower: "", waitlist: [] },
  { id: 15, title: "Head First Java", author: "Kathy Sierra & Bert Bates", available: true, borrower: "", waitlist: [] }
];

const STORAGE_KEYS = { USER: "libtrack_current_user_v1", BOOKS: "libtrack_books_state_v1" };
let currentUser = null;
let books = [];
let activeFilter = "all";
let searchQuery = "";

const authScreen = document.getElementById("authScreen");
const appShell = document.getElementById("appShell");
const loginForm = document.getElementById("loginForm");
const guestBtn = document.getElementById("guestBtn");
const switchAccountBtn = document.getElementById("switchAccountBtn");
const resetDemoBtn = document.getElementById("resetDemoBtn");
const displayUserName = document.getElementById("displayUserName");
const searchInput = document.getElementById("searchInput");
const booksGrid = document.getElementById("booksGrid");
const tabBtns = document.querySelectorAll(".tab-btn");

function init() {
  loadBooks();
  const savedUser = localStorage.getItem(STORAGE_KEYS.USER);
  if (savedUser) {
    currentUser = JSON.parse(savedUser);
    openApp();
  } else {
    authScreen.classList.remove("hidden");
    appShell.classList.add("hidden");
  }
}

function loadBooks() {
  const savedBooks = localStorage.getItem(STORAGE_KEYS.BOOKS);
  if (savedBooks) {
    books = JSON.parse(savedBooks);
  } else {
    books = JSON.parse(JSON.stringify(INITIAL_BOOKS));
    saveBooks();
  }
}

function saveBooks() { localStorage.setItem(STORAGE_KEYS.BOOKS, JSON.stringify(books)); }
function saveUserSession(userObj) { currentUser = userObj; localStorage.setItem(STORAGE_KEYS.USER, JSON.stringify(userObj)); openApp(); }

function openApp() {
  authScreen.classList.add("hidden");
  appShell.classList.remove("hidden");
  displayUserName.textContent = currentUser.isGuest ? "Guest Reader" : `${currentUser.name} (${currentUser.contact})`;
  render();
}

loginForm.addEventListener("submit", (e) => {
  e.preventDefault();
  const name = document.getElementById("userNameInput").value.trim();
  const contact = document.getElementById("userContactInput").value.trim();
  if (!name || !contact) return;
  saveUserSession({ id: contact.toLowerCase(), name, contact, isGuest: false });
  showToast(`Welcome back, ${name}! Session saved.`);
});

guestBtn.addEventListener("click", () => {
  saveUserSession({ id: "guest_reader", name: "Guest Reader", contact: "Guest Mode", isGuest: true });
  showToast("Continuing as Guest. Session saved.");
});

switchAccountBtn.addEventListener("click", () => {
  localStorage.removeItem(STORAGE_KEYS.USER);
  currentUser = null;
  appShell.classList.add("hidden");
  authScreen.classList.remove("hidden");
});

resetDemoBtn.addEventListener("click", () => {
  books = JSON.parse(JSON.stringify(INITIAL_BOOKS));
  saveBooks();
  render();
  showToast("All 15 books restored to Available status.");
});

searchInput.addEventListener("input", (e) => { searchQuery = e.target.value.trim().toLowerCase(); render(); });
tabBtns.forEach((btn) => {
  btn.addEventListener("click", () => {
    tabBtns.forEach((b) => b.classList.remove("active"));
    btn.classList.add("active");
    activeFilter = btn.dataset.filter;
    render();
  });
});

function getUserLabel() { return currentUser.isGuest ? "Guest Reader" : `${currentUser.name} (${currentUser.contact})`; }

window.handleBorrow = function (bookId) {
  const book = books.find((b) => b.id === bookId);
  if (!book) return;
  const userLabel = getUserLabel();
  if (book.available) {
    book.available = false; book.borrower = userLabel; book.borrowerId = currentUser.id;
    saveBooks(); render(); showToast(`Borrowed "${book.title}" successfully!`);
  } else {
    if (book.borrowerId === currentUser.id) { showToast("You already have this book borrowed."); return; }
    if (book.waitlist.some((w) => w.id === currentUser.id)) { showToast("You are already in the waitlist queue for this book."); return; }
    book.waitlist.push({ id: currentUser.id, label: userLabel });
    saveBooks(); render(); showToast(`Added to waitlist queue (Position #${book.waitlist.length}).`);
  }
};

window.handleReturn = function (bookId) {
  const book = books.find((b) => b.id === bookId);
  if (!book || book.available) return;
  if (book.waitlist.length === 0) {
    book.available = true; book.borrower = ""; book.borrowerId = "";
    showToast(`Returned "${book.title}". Now available on shelf.`);
  } else {
    const nextUser = book.waitlist.shift();
    book.borrower = nextUser.label; book.borrowerId = nextUser.id;
    showToast(`Returned & auto-issued to next in queue: ${nextUser.label}`);
  }
  saveBooks(); render();
};

function render() {
  const availableCount = books.filter((b) => b.available).length;
  document.getElementById("statTotal").textContent = books.length;
  document.getElementById("statAvailable").textContent = availableCount;
  document.getElementById("statBorrowed").textContent = books.length - availableCount;
  document.getElementById("statWaitlist").textContent = books.reduce((acc, b) => acc + b.waitlist.length, 0);

  const filtered = books.filter((book) => {
    const matchesSearch = book.id.toString() === searchQuery || book.title.toLowerCase().includes(searchQuery) || book.author.toLowerCase().includes(searchQuery);
    if (!matchesSearch) return false;
    if (activeFilter === "available") return book.available;
    if (activeFilter === "borrowed") return !book.available;
    if (activeFilter === "mybooks") return book.borrowerId === currentUser.id || book.waitlist.some((w) => w.id === currentUser.id);
    return true;
  });

  if (filtered.length === 0) {
    booksGrid.innerHTML = `<div class="stat-card" style="grid-column: 1 / -1; text-align: center; padding: 48px;"><strong style="font-size: 20px; margin-bottom: 6px;">No matching books found</strong><span style="color: var(--text-muted);">Try clearing your search or switching tabs.</span></div>`;
    return;
  }

  booksGrid.innerHTML = filtered.map((book) => {
    const isBorrowedByMe = !book.available && book.borrowerId === currentUser.id;
    const queueIndex = book.waitlist.findIndex((w) => w.id === currentUser.id);
    let statusHtml = !book.available ? `<div class="borrow-meta"><div>Borrowed by: <strong>${escapeHtml(book.borrower)}</strong></div>${book.waitlist.length > 0 ? `<div class="waitlist-tag">Waitlist Queue (${book.waitlist.length}): ${book.waitlist.map((w) => escapeHtml(w.label)).join(" → ")}</div>` : ""}</div>` : "";
    let actionBtn = book.available ? `<button class="btn btn-borrow" onclick="handleBorrow(${book.id})">Borrow Book</button>` : isBorrowedByMe ? `<button class="btn btn-return" onclick="handleReturn(${book.id})">Return Book</button>` : queueIndex !== -1 ? `<button class="btn btn-disabled" disabled>In Waitlist (Position #${queueIndex + 1})</button>` : `<button class="btn btn-waitlist" onclick="handleBorrow(${book.id})">Join Waitlist Queue</button>`;
    return `<article class="book-card"><div><div class="card-top"><span class="book-id">ID #${book.id}</span><span class="badge ${book.available ? "badge-available" : "badge-borrowed"}">${book.available ? "Available" : "Borrowed"}</span></div><h2 class="book-title">${escapeHtml(book.title)}</h2><p class="book-author">by ${escapeHtml(book.author)}</p>${statusHtml}</div><div class="card-actions">${actionBtn}</div></article>`;
  }).join("");
}

function escapeHtml(str) { return String(str).replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;"); }
let toastTimer;
function showToast(msg) { const toast = document.getElementById("toast"); toast.textContent = msg; toast.classList.add("show"); clearTimeout(toastTimer); toastTimer = setTimeout(() => toast.classList.remove("show"), 3200); }

init();

if ('serviceWorker' in navigator) {
  window.addEventListener('load', () => { navigator.serviceWorker.register('./sw.js').catch(() => {}); });
}
let deferredPrompt;
const installAppBtn = document.getElementById('installAppBtn');
window.addEventListener('beforeinstallprompt', (e) => {
  e.preventDefault(); deferredPrompt = e;
  if (installAppBtn) installAppBtn.classList.remove('hidden');
});
if (installAppBtn) {
  installAppBtn.addEventListener('click', async () => {
    if (!deferredPrompt) return;
    deferredPrompt.prompt();
    const { outcome } = await deferredPrompt.userChoice;
    if (outcome === 'accepted') { installAppBtn.classList.add('hidden'); showToast('LibTrack installed on your device!'); }
    deferredPrompt = null;
  });
}
