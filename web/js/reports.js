// Отчёты: амортизация на дату, свод по подразделениям, движения за период.

const MOVEMENT_LABELS = {
    Acquired:       'Поступление',
    Transferred:    'Перемещение',
    SentToRepair:   'На ремонт',
    BackFromRepair: 'Из ремонта',
    WrittenOff:     'Списание',
};

let deptMap = {};

function todayIso() {
    const d = new Date();
    const p = n => String(n).padStart(2, '0');
    return `${d.getFullYear()}-${p(d.getMonth() + 1)}-${p(d.getDate())}`;
}

async function initReports() {
    const today = todayIso();
    document.getElementById('on-date').value = today;
    document.getElementById('to-date').value = today;
    document.getElementById('from-date').value = today.slice(0, 4) + '-01-01';

    document.getElementById('btn-depreciation').addEventListener('click', loadDepreciation);
    document.getElementById('btn-movements').addEventListener('click', loadMovements);
    document.getElementById('btn-csv').addEventListener('click', () => {
        const onDate = document.getElementById('on-date').value || todayIso();
        // Скачивание файла через скрытую ссылку — стратегия CSV на сервере.
        window.location.href = `/api/reports/depreciation?onDate=${onDate}&format=csv`;
    });

    try {
        const depts = await API.get('/api/departments');
        depts.forEach(d => { deptMap[d.id] = d.name; });
    } catch (e) { showError(e.message); }

    loadDepreciation();
    loadCapacity();
    loadMovements();
}

async function loadCapacity() {
    try {
        const rows = await API.get('/api/reports/capacity');
        const tbody = document.getElementById('capacity-body');
        if (!rows.length) {
            tbody.innerHTML = '<tr><td colspan="3" class="empty">Нет производственного оборудования</td></tr>';
            return;
        }
        tbody.innerHTML = rows.map(r => `
            <tr>
                <td>${toyCategoryLabel(r.toyCategory)}</td>
                <td class="num">${r.lineCount}</td>
                <td class="num">${r.totalCapacityPerHour.toLocaleString('ru-RU')}</td>
            </tr>`).join('');
    } catch (e) { showError(e.message); }
}

async function loadDepreciation() {
    const onDate = document.getElementById('on-date').value || todayIso();
    try {
        const [rep, byDept] = await Promise.all([
            API.get('/api/reports/depreciation?onDate=' + onDate),
            API.get('/api/reports/by-department?onDate=' + onDate),
        ]);
        renderDepreciation(rep);
        renderByDept(byDept);
    } catch (e) { showError(e.message); }
}

function renderDepreciation(rep) {
    const tbody = document.getElementById('depreciation-body');
    if (!rep.rows.length) {
        tbody.innerHTML = '<tr><td colspan="6" class="empty">Нет данных</td></tr>';
        return;
    }
    let html = rep.rows.map(r => `
        <tr>
            <td>${esc(r.inventoryNo)}</td>
            <td>${esc(r.name)}</td>
            <td>${categoryLabel(r.category)}</td>
            <td class="num">${formatMoney(r.initialCost)}</td>
            <td class="num">${formatMoney(r.accumulated)}</td>
            <td class="num">${formatMoney(r.residual)}</td>
        </tr>`).join('');
    html += `<tr class="totals-row">
            <td colspan="3">Итого</td>
            <td class="num">${formatMoney(rep.totals.initialCost)}</td>
            <td class="num">${formatMoney(rep.totals.accumulated)}</td>
            <td class="num">${formatMoney(rep.totals.residual)}</td>
        </tr>`;
    tbody.innerHTML = html;
}

function renderByDept(rows) {
    const tbody = document.getElementById('by-dept-body');
    if (!rows.length) {
        tbody.innerHTML = '<tr><td colspan="3" class="empty">Нет данных</td></tr>';
        return;
    }
    tbody.innerHTML = rows.map(s => `
        <tr>
            <td>${esc(s.departmentName)}</td>
            <td class="num">${s.assetCount}</td>
            <td class="num">${formatMoney(s.totalResidual)}</td>
        </tr>`).join('');
}

async function loadMovements() {
    const from = document.getElementById('from-date').value;
    const to = document.getElementById('to-date').value;
    try {
        const data = await API.get(`/api/reports/movements?from=${from}&to=${to}`);
        renderMovements(data);
    } catch (e) { showError(e.message); }
}

function renderMovements(data) {
    const totals = Object.entries(data.totals || {})
        .map(([k, v]) => `${MOVEMENT_LABELS[k] || k}: ${v}`).join(' · ');
    document.getElementById('movements-totals').textContent =
        data.movements.length ? totals : '';

    const tbody = document.getElementById('movements-body');
    if (!data.movements.length) {
        tbody.innerHTML = '<tr><td colspan="6" class="empty">Движений за период нет</td></tr>';
        return;
    }
    tbody.innerHTML = data.movements.map(m => `
        <tr>
            <td>${m.date}</td>
            <td>#${m.assetId}</td>
            <td>${MOVEMENT_LABELS[m.type] || m.type}</td>
            <td>${m.fromDepartmentId ? esc(deptName(m.fromDepartmentId)) : '—'}</td>
            <td>${m.toDepartmentId ? esc(deptName(m.toDepartmentId)) : '—'}</td>
            <td>${esc(m.note || '')}</td>
        </tr>`).join('');
}

function deptName(id) { return deptMap[id] || ('#' + id); }
function esc(s) { return String(s).replace(/[&<>"]/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c])); }

initReports();
