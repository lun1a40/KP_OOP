// Страница фондов: таблица, фильтры, добавление/редактирование, операции.

let departments = [];
let editingId = null;

// --- Инициализация ---
async function init() {
    bindModalClosers();
    document.getElementById('btn-add').addEventListener('click', openAddModal);
    document.getElementById('asset-form').addEventListener('submit', submitAssetForm);
    document.getElementById('form-category').addEventListener('change', onCategoryChange);

    ['f-category', 'f-department', 'f-status'].forEach(id =>
        document.getElementById(id).addEventListener('change', loadAssets));
    document.getElementById('f-query').addEventListener('input', debounce(loadAssets, 300));

    // Категории в фильтр и форму
    const catOptions = Object.entries(CATEGORY_LABELS)
        .map(([v, l]) => `<option value="${v}">${l}</option>`).join('');
    document.getElementById('f-category').insertAdjacentHTML('beforeend', catOptions);
    document.getElementById('form-category').innerHTML = catOptions;

    try {
        departments = await API.get('/api/departments');
    } catch (e) { showError(e.message); }

    const deptOptions = departments.map(d => `<option value="${d.id}">${d.name}</option>`).join('');
    document.getElementById('f-department').insertAdjacentHTML('beforeend', deptOptions);
    document.getElementById('form-department').innerHTML = deptOptions;

    onCategoryChange();
    loadAssets();
}

// --- Загрузка и отрисовка таблицы ---
async function loadAssets() {
    const params = new URLSearchParams();
    const cat = val('f-category'), dep = val('f-department'), st = val('f-status'), q = val('f-query');
    if (cat) params.set('category', cat);
    if (dep) params.set('departmentId', dep);
    if (st)  params.set('status', st);
    if (q)   params.set('q', q);

    try {
        const assets = await API.get('/api/assets?' + params.toString());
        renderTable(assets);
    } catch (e) { showError(e.message); }
}

function renderTable(assets) {
    const tbody = document.getElementById('assets-body');
    if (!assets.length) {
        tbody.innerHTML = '<tr><td colspan="7" class="empty">Фонды не найдены</td></tr>';
        return;
    }
    tbody.innerHTML = assets.map(a => `
        <tr>
            <td>${esc(a.inventoryNo)}</td>
            <td>${esc(a.name)}${a.category === 'production'
                ? `<div style="font-size:12px;color:var(--muted)">${toyCategoryLabel(a.toyCategory)} · ${esc(a.ageGroup || '')}</div>`
                : ''}</td>
            <td>${categoryLabel(a.category)}</td>
            <td>${esc(deptName(a.departmentId))}</td>
            <td><span class="badge ${a.status}">${statusLabel(a.status)}</span></td>
            <td class="num">${formatMoney(a.initialCost)}</td>
            <td><div class="row-actions">${rowActions(a)}</div></td>
        </tr>`).join('');

    tbody.querySelectorAll('button[data-op]').forEach(btn => {
        btn.addEventListener('click', () => handleOp(btn.dataset.op, Number(btn.dataset.id)));
    });
}

function rowActions(a) {
    const b = [];
    if (a.status === 'InUse') {
        b.push(opBtn('transfer', a.id, 'Перевести'));
        b.push(opBtn('send-repair', a.id, 'На ремонт'));
        b.push(opBtn('write-off', a.id, 'Списать', 'danger'));
        b.push(opBtn('edit', a.id, 'Изменить'));
    } else if (a.status === 'InRepair') {
        b.push(opBtn('back-repair', a.id, 'Из ремонта'));
        b.push(opBtn('edit', a.id, 'Изменить'));
    } else { // WrittenOff
        b.push(opBtn('delete', a.id, 'Удалить', 'danger'));
    }
    return b.join('');
}
function opBtn(op, id, label, cls = '') {
    return `<button class="btn sm ${cls}" data-op="${op}" data-id="${id}">${label}</button>`;
}

// --- Операции над фондом ---
async function handleOp(op, id) {
    try {
        if (op === 'edit')        return openEditModal(id);
        if (op === 'delete') {
            if (!confirm('Удалить списанный фонд безвозвратно?')) return;
            await API.del('/api/assets/' + id);
        } else if (op === 'transfer') {
            const asset = await API.get('/api/assets/' + id);
            const opts = departments.filter(d => d.id !== asset.departmentId)
                .map(d => ({ value: d.id, label: d.name }));
            const v = await promptModal('Перемещение фонда', [
                { key: 'toDepartmentId', label: 'Новое подразделение', type: 'select', options: opts },
                { key: 'note', label: 'Примечание', type: 'text' },
            ]);
            if (!v) return;
            await API.post(`/api/assets/${id}/transfer`, { toDepartmentId: Number(v.toDepartmentId), note: v.note });
        } else if (op === 'send-repair') {
            const v = await promptModal('Отправить на ремонт', [{ key: 'note', label: 'Примечание', type: 'text' }]);
            if (!v) return;
            await API.post(`/api/assets/${id}/send-to-repair`, { note: v.note });
        } else if (op === 'back-repair') {
            const v = await promptModal('Возврат из ремонта', [
                { key: 'cost', label: 'Стоимость ремонта, ₽', type: 'number', step: '0.01' },
                { key: 'description', label: 'Что сделано', type: 'text' },
            ]);
            if (!v) return;
            await API.post(`/api/assets/${id}/back-from-repair`,
                { cost: rublesToKopecks(v.cost || 0), description: v.description });
        } else if (op === 'write-off') {
            const v = await promptModal('Списание фонда', [{ key: 'reason', label: 'Причина списания', type: 'text' }]);
            if (!v) return;
            await API.post(`/api/assets/${id}/write-off`, { reason: v.reason });
        }
        loadAssets();
    } catch (e) { showError(e.message); }
}

// --- Модалка добавления / редактирования ---
function openAddModal() {
    editingId = null;
    document.getElementById('asset-modal-title').textContent = 'Новый фонд';
    const form = document.getElementById('asset-form');
    form.reset();
    document.getElementById('form-category').disabled = false;
    onCategoryChange();
    openModal('asset-modal');
}

async function openEditModal(id) {
    try {
        const a = await API.get('/api/assets/' + id);
        editingId = id;
        document.getElementById('asset-modal-title').textContent = 'Редактирование фонда';
        const form = document.getElementById('asset-form');
        form.inventoryNo.value = a.inventoryNo;
        form.name.value = a.name;
        form.category.value = a.category;
        document.getElementById('form-category').disabled = true;   // категорию менять нельзя
        form.departmentId.value = a.departmentId;
        form.initialCostRub.value = (a.initialCost / 100).toFixed(2);
        form.acquiredOn.value = a.acquiredOn;
        form.usefulLifeMonths.value = a.usefulLifeMonths;
        buildCatFields(a.category, a);
        openModal('asset-modal');
    } catch (e) { showError(e.message); }
}

function onCategoryChange() {
    const cat = document.getElementById('form-category').value;
    buildCatFields(cat, {});
}

function buildCatFields(category, values) {
    const fields = CATEGORY_FIELDS[category] || [];
    document.getElementById('cat-fields').innerHTML = fields.map(f => {
        if (f.type === 'select') {
            const cur = values[f.key];
            const opts = f.options.map(o =>
                `<option value="${o.value}" ${o.value === cur ? 'selected' : ''}>${esc(o.label)}</option>`).join('');
            return `<div class="field"><label>${f.label}</label>
                    <select name="${f.key}" required>${opts}</select></div>`;
        }
        return `<div class="field">
            <label>${f.label}</label>
            <input name="${f.key}" type="${f.type}" ${f.step ? `step="${f.step}"` : ''}
                   value="${values[f.key] !== undefined ? esc(String(values[f.key])) : ''}"
                   ${f.type === 'number' ? 'min="0"' : ''} required>
        </div>`;
    }).join('');
}

async function submitAssetForm(e) {
    e.preventDefault();
    const form = e.target;
    const category = form.category.value;
    const body = {
        inventoryNo: form.inventoryNo.value.trim(),
        name: form.name.value.trim(),
        category,
        departmentId: Number(form.departmentId.value),
        initialCost: rublesToKopecks(form.initialCostRub.value),
        acquiredOn: form.acquiredOn.value,
        usefulLifeMonths: Number(form.usefulLifeMonths.value),
    };
    (CATEGORY_FIELDS[category] || []).forEach(f => {
        const el = form.elements[f.key];
        body[f.key] = f.type === 'number' ? Number(el.value) : el.value.trim();
    });

    try {
        if (editingId) await API.put('/api/assets/' + editingId, body);
        else           await API.post('/api/assets', body);
        closeModal('asset-modal');
        loadAssets();
    } catch (err) { showError(err.message); }
}

// --- Универсальная модалка операции (возвращает Promise) ---
function promptModal(title, fields) {
    return new Promise(resolve => {
        document.getElementById('op-modal-title').textContent = title;
        document.getElementById('op-fields').innerHTML = fields.map(f => {
            if (f.type === 'select') {
                const opts = f.options.map(o => `<option value="${o.value}">${esc(o.label)}</option>`).join('');
                return `<div class="field"><label>${f.label}</label><select name="${f.key}">${opts}</select></div>`;
            }
            return `<div class="field"><label>${f.label}</label>
                    <input name="${f.key}" type="${f.type}" ${f.step ? `step="${f.step}"` : ''}></div>`;
        }).join('');

        const form = document.getElementById('op-form');
        const onSubmit = (e) => {
            e.preventDefault();
            const values = {};
            fields.forEach(f => { values[f.key] = form.elements[f.key].value; });
            cleanup();
            closeModal('op-modal');
            resolve(values);
        };
        const onCancel = () => { cleanup(); resolve(null); };
        function cleanup() {
            form.removeEventListener('submit', onSubmit);
            opCancelHandlers = opCancelHandlers.filter(h => h !== onCancel);
        }
        form.addEventListener('submit', onSubmit);
        opCancelHandlers.push(onCancel);
        openModal('op-modal');
    });
}

// --- Вспомогательное ---
let opCancelHandlers = [];

function bindModalClosers() {
    document.querySelectorAll('[data-close]').forEach(el => {
        el.addEventListener('click', () => {
            const id = el.dataset.close;
            closeModal(id);
            if (id === 'op-modal') opCancelHandlers.forEach(h => h());
        });
    });
}
function openModal(id)  { document.getElementById(id).classList.add('open'); }
function closeModal(id) { document.getElementById(id).classList.remove('open'); }

function deptName(id) {
    const d = departments.find(x => x.id === id);
    return d ? d.name : ('#' + id);
}
function val(id) { return document.getElementById(id).value.trim(); }
function esc(s) { return String(s).replace(/[&<>"]/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c])); }
function debounce(fn, ms) { let t; return (...a) => { clearTimeout(t); t = setTimeout(() => fn(...a), ms); }; }

init();
