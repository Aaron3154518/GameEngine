import {
  AfterViewInit,
  Component,
  ComponentRef,
  DoCheck,
  ElementRef,
  EventEmitter,
  Input,
  IterableDiffer,
  IterableDiffers,
  Output,
  QueryList,
  Type,
  ViewChild,
  ViewChildren,
  ViewContainerRef,
} from '@angular/core';
import { ContainerDirective } from '../directives/container.directive';
import { InputComponent } from './input/input.component';
import { StringDict } from '../utils/interfaces';
import { sanitizeVar } from '../utils/utils';

export interface ColComponent {
  row?: any;
  value?: any;
}

@Component({
  selector: 'col-header',
  templateUrl: './col-header.component.html',
})
export class ColHeaderComponent implements ColComponent {
  @Input() value: string = '';
  @Input() replaceClasses: boolean = false;

  @Input() set classes(cls: string[]) {
    if (this.replaceClasses) {
      this._classes = cls;
      return;
    }

    this._classes = this._classes.concat(cls);
  }

  get classes(): string[] {
    return this._classes;
  }

  _classes: string[] = [
    'input-group-text',
    'd-inline-block',
    'text-start',
    'py-0',
    'px-1',
    'w-100',
  ];
}

export enum ColWidth {
  Fit = 'w-auto',
  Fill = 'w-100',
}

interface IColumn {
  id?: string;
  key: string;
  getter?: (row: any) => string;
  component: Type<any>;
  width?: ColWidth;
  requireInput?: boolean;
  inputClasses?: string[];
  inputPlaceholder?: string;
  validateInput?: (val: string) => boolean;
  sanitizeInput?: (val: string) => string;
}

export class Column implements IColumn {
  key: string;
  getter?: (row: any) => string;
  component: Type<any>;
  width: ColWidth;
  requireInput: boolean;
  inputClasses: string[];
  inputPlaceholder: string;
  validateInput: (val: string) => boolean;
  sanitizeInput: (val: string) => string;

  constructor({
    key,
    component,
    getter,
    width = ColWidth.Fill,
    requireInput = false,
    inputClasses = [],
    inputPlaceholder = '',
    validateInput = () => true,
    sanitizeInput = (s: string) => s,
  }: IColumn) {
    this.key = key;
    this.getter = getter;
    this.component = component;
    this.width = width;
    this.requireInput = requireInput;
    this.inputClasses = inputClasses;
    this.inputPlaceholder = inputPlaceholder;
    this.validateInput = validateInput;
    this.sanitizeInput = sanitizeInput;
  }
}

@Component({
  selector: 'app-search',
  templateUrl: './search.component.html',
  styleUrls: ['./search.component.css'],
})
export class SearchComponent implements DoCheck, AfterViewInit {
  @Input() data: readonly any[] = [];
  @Input() cols: Column[] = [];
  @Input() allowNew: boolean = false;
  @Input() sort: (rows: any[], query: string) => void = () => {};
  @Input() searchPlaceholder: string = '';
  @Input() dragTarget: boolean = false;
  @Input() trash: boolean = false;

  @Output() onTrash: EventEmitter<DragEvent> = new EventEmitter();

  @Output() newRow: EventEmitter<StringDict<string>> = new EventEmitter();

  @ViewChild('search', { static: true }) search?: ElementRef<HTMLInputElement>;
  @ViewChildren('rowInput') colInputComps?: QueryList<InputComponent>;

  rows: any[] = [];

  colInputs: StringDict<string> = {};
  newRowErrs: { [key: string]: boolean } = {};

  iterableDiffer: IterableDiffer<any>;

  draggingOver: number[] = [];

  dragCnt: number = 0;

  sanitizeVar: (s: string) => string = sanitizeVar;

  constructor(iterableDiffers: IterableDiffers) {
    this.iterableDiffer = iterableDiffers.find([]).create(undefined);
  }

  ngDoCheck(): void {
    if (this.iterableDiffer.diff(this.data)) {
      this.rows = [...this.data];
      this.draggingOver = this.rows.map(() => 0);
      if (this.search) {
        this.onSearchChanged(this.search.nativeElement.value);
      }
    }
  }

  ngAfterViewInit(): void {
    setTimeout(() => {
      this.colInputComps?.forEach((ic: InputComponent) => (ic.value = ''));
    }, 0);
  }

  init(container: ContainerDirective, row: any, col: Column): void {
    const viewContainerRef: ViewContainerRef = container.viewContainerRef;
    viewContainerRef.clear();

    const componentRef: ComponentRef<ColComponent> =
      viewContainerRef.createComponent<ColComponent>(col.component);
    componentRef.instance.row = row;
    componentRef.instance.value = this.getCol(row, col);
  }

  getCol(row: any, col: Column): string {
    return col.getter ? col.getter(row) : row[col.key];
  }

  onSearchChanged(query: string) {
    this.sort(this.rows, query);
  }

  onInput(col: Column) {
    this.newRowErrs[col.key] =
      !!this.colInputs[col.key] && !col.validateInput(this.colInputs[col.key]);
  }

  onEnter() {
    if (Object.values(this.newRowErrs).findIndex((b: boolean) => b) !== -1) {
      return;
    }

    this.newRow.next(this.colInputs);
    this.colInputComps?.forEach((ic: InputComponent) => {
      ic.value = '';
    });
    this.colInputComps?.first.select();
  }

  onDragChange(i: number, v: number) {
    if (!this.dragTarget) {
      return;
    }
    if (v === 0) {
      this.draggingOver = this.rows.map(() => 0);
      return;
    }
    this.draggingOver[i] += v;
  }

  onTrashDrop(event: DragEvent) {
    event.preventDefault();
    if (event.dataTransfer) {
      this.onTrash.next(event);
    }
  }

  onTrashDragOver(event: DragEvent) {
    event.preventDefault();
    event.stopPropagation();
  }
}
