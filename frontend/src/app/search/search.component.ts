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
  OnChanges,
  Output,
  QueryList,
  SimpleChanges,
  Type,
  ViewChild,
  ViewChildren,
  ViewContainerRef,
} from '@angular/core';
import { ContainerDirective } from '../directives/container.directive';
import { InputComponent } from './input/input.component';
import { StringDict } from '../utils/interfaces';
import { getAttr, sanitizeVar } from '../utils/utils';
import { ColComponent } from '../parameter-views/parameter-views.component';

interface IColumn {
  id?: string;
  key: string;
  getter?: (row: any) => any;
  component: Type<any>;
  width?: number;
  cellClasses?: string[];
  requireInput?: boolean;
  inputClasses?: string[];
  inputPlaceholder?: string;
  validateInput?: (val: string) => boolean;
  sanitizeInput?: (val: string, i: number) => string;
}

export class Column implements IColumn {
  key: string;
  getter: (row: any) => any;
  readonly component: Type<any>;
  width: number;
  cellClasses: string[];
  requireInput: boolean;
  inputClasses: string[];
  inputPlaceholder: string;
  validateInput: (val: string) => boolean;
  sanitizeInput: (val: string, i: number) => string;

  constructor({
    key,
    component,
    getter = (row: any) => getAttr(row, key),
    width = 1,
    cellClasses = [],
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
    this.cellClasses = cellClasses;
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
export class SearchComponent implements DoCheck, AfterViewInit, OnChanges {
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

  protected colInputs: StringDict<string> = {};
  protected newRowErrs: { [key: string]: boolean } = {};

  private colWidthSum: number = 0;

  protected iterableDiffer: IterableDiffer<any>;

  protected draggingOver: number[] = [];

  protected dragCnt: number = 0;

  constructor(iterableDiffers: IterableDiffers) {
    this.iterableDiffer = iterableDiffers.find([]).create(undefined);
  }

  ngOnChanges(changes: SimpleChanges): void {
    if (changes['cols']) {
      this.colWidthSum = this.cols
        .filter((col: Column) => col.width >= 0)
        .reduce((s: number, col: Column) => s + col.width, 0);
    }
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
    componentRef.instance.value = col.getter(row);
  }

  getWidthStyle(w: number): string {
    return w === 0 ? 'auto' : `${(w * 100) / this.colWidthSum}%`;
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
