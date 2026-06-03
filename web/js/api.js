// Тонкая обёртка над fetch + общие справочники и форматтеры.
// Все суммы в API — целое число копеек; на фронте конвертируем в рубли.

const API = {
    async request(method, path, body) {
        const opts = { method, headers: {} };
        if (body !== undefined) {
            opts.headers['Content-Type'] = 'application/json';
            opts.body = JSON.stringify(body);
        }
        const res = await fetch(path, opts);
        if (res.status === 204) return null;
        const text = await res.text();
        const data = text ? JSON.parse(text) : null;
        if (!res.ok) {
            const msg = data && data.details ? data.details : `Ошибка ${res.status}`;
            throw new Error(msg);
        }
        return data;
    },
    get(path)        { return this.request('GET', path); },
    post(path, body) { return this.request('POST', path, body); },
    put(path, body)  { return this.request('PUT', path, body); },
    del(path)        { return this.request('DELETE', path); },
};

// --- Справочники ярлыков ---
const CATEGORY_LABELS = {
    production: 'Производство',
    building:   'Здание',
    vehicle:    'Транспорт',
    office:     'Офисная техника',
};
const STATUS_LABELS = {
    InUse:      'В работе',
    InRepair:   'В ремонте',
    WrittenOff: 'Списан',
};

// Виды выпускаемых игрушек и возрастные маркировки (ТР ТС 008/2011).
const TOY_CATEGORY_LABELS = {
    plastic:    'Пластиковая',
    plush:      'Мягкая',
    wooden:     'Деревянная',
    board:      'Настольная игра',
    electronic: 'Электронная',
};
const TOY_CATEGORY_OPTIONS = Object.entries(TOY_CATEGORY_LABELS).map(([v, l]) => ({ value: v, label: l }));
const AGE_GROUP_OPTIONS = ['0+', '3+', '6+', '14+'].map(v => ({ value: v, label: v }));

function toyCategoryLabel(c) { return TOY_CATEGORY_LABELS[c] || c; }

// Специфичные поля по категории (для модалки добавления/редактирования).
const CATEGORY_FIELDS = {
    production: [
        { key: 'model',           label: 'Модель',                      type: 'text' },
        { key: 'toyCategory',     label: 'Вид игрушки',                 type: 'select', options: TOY_CATEGORY_OPTIONS },
        { key: 'ageGroup',        label: 'Возрастная маркировка',       type: 'select', options: AGE_GROUP_OPTIONS },
        { key: 'capacityPerHour', label: 'Производительность, игрушек/ч', type: 'number' },
        { key: 'runHours',        label: 'Наработка, ч',                type: 'number' },
    ],
    building: [
        { key: 'area',    label: 'Площадь, м²', type: 'number', step: '0.01' },
        { key: 'address', label: 'Адрес',       type: 'text' },
    ],
    vehicle: [
        { key: 'licensePlate',         label: 'Госномер',          type: 'text' },
        { key: 'mileage',              label: 'Пробег, км',        type: 'number' },
        { key: 'expectedTotalMileage', label: 'Ресурс пробега, км', type: 'number' },
    ],
    office: [
        { key: 'serialNumber', label: 'Серийный номер', type: 'text' },
    ],
};

// --- Форматтеры ---
function formatMoney(kopecks) {
    const rub = (kopecks / 100);
    return rub.toLocaleString('ru-RU', { minimumFractionDigits: 2, maximumFractionDigits: 2 }) + ' ₽';
}
function rublesToKopecks(rubles) {
    return Math.round(parseFloat(rubles) * 100);
}
function categoryLabel(c) { return CATEGORY_LABELS[c] || c; }
function statusLabel(s)   { return STATUS_LABELS[s] || s; }

function showError(message) {
    let box = document.getElementById('error-box');
    if (!box) return alert(message);
    box.textContent = message;
    box.classList.remove('hidden');
    setTimeout(() => box.classList.add('hidden'), 6000);
}

// Подсветка активного пункта меню по имени файла.
function highlightNav() {
    const here = location.pathname.split('/').pop() || 'index.html';
    document.querySelectorAll('nav.menu a').forEach(a => {
        if (a.getAttribute('href') === here) a.classList.add('active');
    });
}
document.addEventListener('DOMContentLoaded', highlightNav);
