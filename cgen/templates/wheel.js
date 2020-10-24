
const zoom_speed = 0.5;
const pan_speed = 20;
const drawing = document.querySelector('#drawing');
const svg = document.querySelector('svg');

let translate = [drawing.transform.baseVal[0].matrix.e, drawing.transform.baseVal[0].matrix.f];
let scale = drawing.transform.baseVal[1].matrix.a;

let drag_start_pos = [0, 0];

function set_transform()
{
    const [tx, ty] = translate;
    const s = scale; 
    drawing.attributes.transform.value = `translate(${tx}, ${ty}) scale(${s}, ${s})`;
}

function zoom_in(pos)
{
    const zs = zoom_speed;
    const [x, y] = pos;
    const [dtx, dty] = [-zs * x, -zs * y];
    const [tx, ty] = translate;
    translate = [tx + dtx, ty + dty];
    scale += zs;
    set_transform();
}

function zoom_out(pos)
{
    if (scale < 2)
    {
        return;
    }
    const zs = -zoom_speed;
    const [x, y] = pos;
    const [dtx, dty] = [-zs * x, -zs * y];
    const [tx, ty] = translate;
    translate = [tx + dtx, ty + dty];
    scale += zs;
    set_transform();
}

function get_absolute_pos(evt)
{
    const CTM = drawing.getScreenCTM();
    const x = Math.round((evt.clientX - CTM.e) / CTM.a);
    const y = Math.round((evt.clientY - CTM.f) / CTM.d);

    return [x, y];
}



svg.addEventListener('wheel', event => {
    if (event.ctrlKey)
    {
        event.stopPropagation();
        event.preventDefault();
        const p = get_absolute_pos(event);
        if (event.deltaY < 0)
        {
            zoom_in(p);
        }
        else
        {
            zoom_out(p);
        }
    }
    else if (event.shiftKey)
    {
        event.stopPropagation();
        event.preventDefault();
        if (event.deltaY < 0)
        {
            translate[0] += pan_speed;
        }
        else
        {
            translate[0] -= pan_speed;
        }
        set_transform();
    }
    else
    {
        event.stopPropagation();
        event.preventDefault();
        if (event.deltaY < 0)
        {
            translate[1] += pan_speed;
        }
        else
        {
            translate[1] -= pan_speed;
        }
        set_transform();
    }
});